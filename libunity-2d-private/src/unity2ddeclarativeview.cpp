/*
 * Copyright (C) 2011 Canonical, Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 3.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "unity2ddeclarativeview.h"

#include <debug_p.h>
#include <config.h>

#include "screeninfo.h"
#include "gobjectcallback.h"

#include <QDebug>
#include <QGLWidget>
#include <QVariant>
#include <QX11Info>
#include <QFileInfo>
#include <QShowEvent>

#include <X11/Xlib.h>
#include <X11/Xatom.h>

#include "bamf-window.h"
#include "bamf-matcher.h"

// libwnck
extern "C" {
#include <libwnck/libwnck.h>
}

GOBJECT_CALLBACK1(activeWorkspaceChangedCB, "onActiveWorkspaceChanged");

Unity2DDeclarativeView::Unity2DDeclarativeView(QWidget *parent) :
    QDeclarativeView(parent),
    m_screenInfo(NULL),
    m_useOpenGL(false),
    m_transparentBackground(false),
    m_last_focused_window(None)
{
    if (!QFileInfo(UNITY_2D_SCHEMA_FILE).exists()) {
        m_useOpenGL = false;
    } else {
        m_useOpenGL = unity2dConfiguration().property("useOpengl").toBool();
    }

    WnckScreen* screen = wnck_screen_get_default();
    g_signal_connect(G_OBJECT(screen), "active_workspace_changed", G_CALLBACK(activeWorkspaceChangedCB), this);

    setupViewport();
}

Unity2DDeclarativeView::~Unity2DDeclarativeView()
{
    WnckScreen* screen = wnck_screen_get_default();
    g_signal_handlers_disconnect_by_func(G_OBJECT(screen), gpointer(activeWorkspaceChangedCB), this);
}

bool Unity2DDeclarativeView::useOpenGL() const
{
    return m_useOpenGL;
}

void Unity2DDeclarativeView::setUseOpenGL(bool useOpenGL)
{
    if (useOpenGL == m_useOpenGL) {
        return;
    }

    m_useOpenGL = useOpenGL;
    setupViewport();

    Q_EMIT useOpenGLChanged(useOpenGL);
}

bool Unity2DDeclarativeView::transparentBackground() const
{
    return m_transparentBackground;
}

void Unity2DDeclarativeView::setTransparentBackground(bool transparentBackground)
{
    if (transparentBackground == m_transparentBackground) {
        return;
    }

    m_transparentBackground = transparentBackground;
    setupViewport();

    Q_EMIT transparentBackgroundChanged(transparentBackground);
}

QPoint Unity2DDeclarativeView::globalPosition() const
{
    return mapToGlobal(QPoint(0,0));
}

void Unity2DDeclarativeView::setupViewport()
{
    if (m_useOpenGL) {
        QGLFormat format = QGLFormat::defaultFormat();
        format.setSampleBuffers(false);
        /* Synchronise rendering with vblank */
        format.setSwapInterval(1);

        QGLWidget *glWidget = new QGLWidget(format);
        /* TODO: possibly faster, to be tested, only works with non transparent windows */
        //glWidget->setAutoFillBackground(false);

        if (m_transparentBackground) {
            glWidget->setAttribute(Qt::WA_TranslucentBackground, true);
            setAttribute(Qt::WA_TranslucentBackground, true);
            /* automatically set by setting WA_TranslucentBackground */
            setAttribute(Qt::WA_NoSystemBackground, true);
            setAttribute(Qt::WA_OpaquePaintEvent, true);
        } else {
            glWidget->setAttribute(Qt::WA_TranslucentBackground, false);
            setAttribute(Qt::WA_TranslucentBackground, false);
            setAttribute(Qt::WA_NoSystemBackground, true);
            setAttribute(Qt::WA_OpaquePaintEvent, true);
        }

        setViewport(glWidget);
        /* According to Qt's documentation: "This is the preferred update mode
           for viewports that do not support partial updates, such as QGLWidget [...]"
        */
        setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
    } else {
        setViewport(0);
        /* This is the default update mode */
        setViewportUpdateMode(QGraphicsView::MinimalViewportUpdate);

        if (m_transparentBackground) {
            viewport()->setAttribute(Qt::WA_TranslucentBackground, true);
            setAttribute(Qt::WA_TranslucentBackground, true);
            /* automatically set by setting WA_TranslucentBackground */
            setAttribute(Qt::WA_NoSystemBackground, true);
            setAttribute(Qt::WA_OpaquePaintEvent, false);
        } else {
            viewport()->setAttribute(Qt::WA_TranslucentBackground, false);
            setAttribute(Qt::WA_TranslucentBackground, false);
            setAttribute(Qt::WA_NoSystemBackground, true);
            setAttribute(Qt::WA_OpaquePaintEvent, true);
        }
    }
}

void Unity2DDeclarativeView::moveEvent(QMoveEvent* event)
{
    Q_EMIT globalPositionChanged(globalPosition());
}

void Unity2DDeclarativeView::showEvent(QShowEvent* event)
{
    QDeclarativeView::showEvent(event);
    Q_EMIT visibleChanged(true);
}

void Unity2DDeclarativeView::hideEvent(QHideEvent* event)
{
    QDeclarativeView::hideEvent(event);
    Q_EMIT visibleChanged(false);
}

/* Obtaining & Discarding Keyboard Focus for Window on Demand
 *
 * In the X world, activating a window means to give it the input (keyboard)
 * focus. When a new window opens, X usually makes it active immediately.
 * Clicking on a window makes it active too.
 *
 * Qt does not have the capability to explicitly ask the window manager to
 * make an existing window active - setFocus() only forwards input focus to
 * whatever QWidget you specify.
 *
 * De-Activating a window is not possible with X (and hence with Qt). So
 * we work-around this by remembering which application is active prior to
 * stealing focus, and then Re-Activating it when we're finished. This is
 * not guaranteed to succeed, as previous window may have closed.
 *
 * The following methods deal with these tasks. Note that when the window
 * has been activated (deactivated), Qt will realise it has obtained (lost)
 * focus and act appropriately.
 */

/* Ask Window Manager to activate this window and hence get keyboard focus */
void Unity2DDeclarativeView::forceActivateWindow()
{
    // Save reference to window with current keyboard focus
    if( m_last_focused_window == None ){
        saveActiveWindow();
    }

    // Show this window by giving it keyboard focus
    forceActivateThisWindow(this->effectiveWinId());
}

/* Ask Window Manager to deactivate this window - not guaranteed to succeed. */
void Unity2DDeclarativeView::forceDeactivateWindow()
{
    if( m_last_focused_window == None ){
        UQ_WARNING << "No previously focused window found, use mouse to select window.";
        return;
    }

    // What if previously focused window closed while we we had focus? Check if window
    // exists by seeing if it has attributes.
    int status;
    XWindowAttributes attributes;
    status = XGetWindowAttributes(QX11Info::display(), m_last_focused_window, &attributes);
    if ( status == BadWindow ){
        UQ_WARNING << "Previously focused window has gone, use mouse to select window.";
        return;
    }

    // Show this window by giving it keyboard focus
    forceActivateThisWindow(m_last_focused_window);

    m_last_focused_window = None;
    Q_EMIT lastFocusedWindowChanged(m_last_focused_window);
}

void Unity2DDeclarativeView::forceActivateThisWindow(WId window)
{
    /* Workaround focus stealing prevention implemented by some window
       managers such as Compiz. This is the exact same code you will find in
       libwnck::wnck_window_activate().

       ref.: http://permalink.gmane.org/gmane.comp.lib.qt.general/4733
    */
    Display* display = QX11Info::display();
    Atom net_wm_active_window = XInternAtom(display, "_NET_ACTIVE_WINDOW",
                                            False);
    XEvent xev;
    xev.xclient.type = ClientMessage;
    xev.xclient.send_event = True;
    xev.xclient.display = display;
    xev.xclient.window = window;
    xev.xclient.message_type = net_wm_active_window;
    xev.xclient.format = 32;
    xev.xclient.data.l[0] = 2;
    xev.xclient.data.l[1] = CurrentTime;
    xev.xclient.data.l[2] = 0;
    xev.xclient.data.l[3] = 0;
    xev.xclient.data.l[4] = 0;

    XSendEvent(display, QX11Info::appRootWindow(), False,
               SubstructureRedirectMask | SubstructureNotifyMask, &xev);

    /* Ensure focus is actually switched to active window */
    XSetInputFocus(display, window, RevertToParent, CurrentTime);
    XFlush(display);

    /* Use Qt's setFocus mechanism as a safety guard in case the above failed */
    setFocus();
}

/* Save WId of window with keyboard focus to m_last_focused_window */
void Unity2DDeclarativeView::saveActiveWindow()
{
    /* Using Bamf here, 'cause XGetFocusInputFocus returned a XId
       different by 1, which then could not be used with Bamf to
       get the application. The change does not result in any functional
       differences, though. */
    WId active_window = None;
    BamfWindow* bamf_active_window = BamfMatcher::get_default().active_window();

    /* Bamf can return a null active window - example case is just after 
       login when no application has been yet been started. */
    if (bamf_active_window != NULL) {
        active_window = bamf_active_window->xid();
    }

    if (active_window != this->effectiveWinId() && active_window != m_last_focused_window) {
        m_last_focused_window = active_window;
        Q_EMIT lastFocusedWindowChanged(m_last_focused_window);
    }
}

void Unity2DDeclarativeView::onActiveWorkspaceChanged() 
{
    m_last_focused_window = None;
    Q_EMIT activeWorkspaceChanged();
}

ScreenInfo*
Unity2DDeclarativeView::screen() const
{
    return m_screenInfo;
}

unsigned int
Unity2DDeclarativeView::lastFocusedWindow() const
{
    return m_last_focused_window;
}

#include <unity2ddeclarativeview.moc>
