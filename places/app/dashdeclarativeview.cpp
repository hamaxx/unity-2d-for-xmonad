/*
 * Copyright (C) 2010 Canonical, Ltd.
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

#include "dashdeclarativeview.h"
#include <QDesktopWidget>
#include <QApplication>
#include <QBitmap>
#include <QCloseEvent>
#include <QDeclarativeContext>
#include <QX11Info>
#include <QGraphicsObject>

#include <QDebug>

#include <X11/Xlib.h>
#include <X11/Xatom.h>

#include <config.h>

static const int DASH_MIN_SCREEN_WIDTH = 1280;
static const int DASH_MIN_SCREEN_HEIGHT = 1084;

static const int DASH_DESKTOP_WIDTH = 989;
static const int DASH_DESKTOP_COLLAPSED_HEIGHT = 78;
static const int DASH_DESKTOP_EXPANDED_HEIGHT = 606;

DashDeclarativeView::DashDeclarativeView()
: QDeclarativeView()
, m_mode(HiddenMode)
, m_expanded(false)
{
    /* The dash window is borderless and not moveable by the user, yet not
       fullscreen */
    setAttribute(Qt::WA_X11NetWmWindowTypeDock, true);

    if (QX11Info::isCompositingManagerRunning()) {
        setAttribute(Qt::WA_TranslucentBackground);
        viewport()->setAttribute(Qt::WA_TranslucentBackground);
    } else {
        QPalette pal = palette();
        // .51 is the alpha of the normal background, turned into the value to
        // be opaque
        pal.setColor(backgroundRole(), QColor::fromHsvF(0, 0, .51));
        setPalette(pal);
    }

    QDesktopWidget* desktop = QApplication::desktop();
    connect(desktop, SIGNAL(resized(int)), SIGNAL(screenGeometryChanged()));
    connect(desktop, SIGNAL(workAreaResized(int)), SLOT(onWorkAreaResized(int)));
}

void
DashDeclarativeView::onWorkAreaResized(int screen)
{
    if (QApplication::desktop()->screenNumber(this) != screen) {
        return;
    }

    if (m_mode == FullScreenMode) {
        fitToAvailableSpace();
    }

    availableGeometryChanged();
}

void
DashDeclarativeView::fitToAvailableSpace()
{
    setGeometry(QApplication::desktop()->availableGeometry(this));
}

void
DashDeclarativeView::resizeToDesktopModeSize()
{
    QRect rect = QApplication::desktop()->availableGeometry(this);

    rect.setWidth(DASH_DESKTOP_WIDTH);
    rect.setHeight(m_expanded ? DASH_DESKTOP_EXPANDED_HEIGHT : DASH_DESKTOP_COLLAPSED_HEIGHT);
    setGeometry(rect);
}

void
DashDeclarativeView::focusOutEvent(QFocusEvent* event)
{
    QDeclarativeView::focusOutEvent(event);
    setActive(false);
}

static int getenvInt(const char* name, int defaultValue)
{
    QByteArray stringValue = qgetenv(name);
    bool ok;
    int value = stringValue.toInt(&ok);
    return ok ? value : defaultValue;
}

void
DashDeclarativeView::setActive(bool value)
{
    if (value != active()) {
        if (value) {
            QRect rect = QApplication::desktop()->screenGeometry(this);
            static int minWidth = getenvInt("DASH_MIN_SCREEN_WIDTH", DASH_MIN_SCREEN_WIDTH);
            static int minHeight = getenvInt("DASH_MIN_SCREEN_HEIGHT", DASH_MIN_SCREEN_HEIGHT);
            if (rect.width() <= minWidth && rect.height() <= minHeight) {
                setDashMode(FullScreenMode);
            } else {
                setDashMode(DesktopMode);
            }
        } else {
            setDashMode(HiddenMode);
        }
    }
}

bool
DashDeclarativeView::active() const
{
    return m_mode != HiddenMode;
}

void
DashDeclarativeView::setDashMode(DashDeclarativeView::DashMode mode)
{
    if (m_mode == mode) {
        return;
    }

    m_mode = mode;
    if (m_mode == HiddenMode) {
        hide();
        activeChanged(false);
    } else {
        show();
        raise();
        activateWindow();
        forceActivateWindow();
        if (m_mode == FullScreenMode) {
            fitToAvailableSpace();
        } else {
            resizeToDesktopModeSize();
        }
        activeChanged(true);
    }
    dashModeChanged(m_mode);
}

DashDeclarativeView::DashMode
DashDeclarativeView::dashMode() const
{
    return m_mode;
}

void
DashDeclarativeView::setExpanded(bool value)
{
    if (m_expanded == value) {
        return;
    }
    
    m_expanded = value;
    if (m_mode == DesktopMode) {
        resizeToDesktopModeSize();
    }
    expandedChanged(m_expanded);
}

bool
DashDeclarativeView::expanded() const
{
    return m_expanded;
}

void
DashDeclarativeView::setActivePlaceEntry(const QString& activePlaceEntry)
{
    if (activePlaceEntry != m_activePlaceEntry) {
        m_activePlaceEntry = activePlaceEntry;
        Q_EMIT activePlaceEntryChanged(activePlaceEntry);
    }
}

const QString&
DashDeclarativeView::activePlaceEntry() const
{
    return m_activePlaceEntry;
}

void
DashDeclarativeView::forceActivateWindow()
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
    xev.xclient.window = this->effectiveWinId();
    xev.xclient.message_type = net_wm_active_window;
    xev.xclient.format = 32;
    xev.xclient.data.l[0] = 2;
    xev.xclient.data.l[1] = CurrentTime;
    xev.xclient.data.l[2] = 0;
    xev.xclient.data.l[3] = 0;
    xev.xclient.data.l[4] = 0;

    XSendEvent(display, QX11Info::appRootWindow(), False,
               SubstructureRedirectMask | SubstructureNotifyMask, &xev);
}

void
DashDeclarativeView::activatePlaceEntry(const QString& file, const QString& entry, const int section)
{
    QGraphicsObject* dash = rootObject();
    QMetaObject::invokeMethod(dash, "activatePlaceEntry", Qt::AutoConnection,
                              Q_ARG(QVariant, QVariant::fromValue(file)),
                              Q_ARG(QVariant, QVariant::fromValue(entry)),
                              Q_ARG(QVariant, QVariant::fromValue(section)));
    setActive(true);
}

void
DashDeclarativeView::activateHome()
{
    QGraphicsObject* dash = rootObject();
    QMetaObject::invokeMethod(dash, "activateHome", Qt::AutoConnection);
    setActive(true);
}

const QRect
DashDeclarativeView::screenGeometry() const
{
    QDesktopWidget* desktop = QApplication::desktop();
    return desktop->screenGeometry(this);
}

const QRect
DashDeclarativeView::availableGeometry() const
{
    QDesktopWidget* desktop = QApplication::desktop();
    return desktop->availableGeometry(this);
}

void
DashDeclarativeView::keyPressEvent(QKeyEvent* event)
{
    switch (event->key()) {
        case Qt::Key_Escape:
            setActive(false);
            break;
        default:
            QDeclarativeView::keyPressEvent(event);
            break;
    }
}

void
DashDeclarativeView::resizeEvent(QResizeEvent* event)
{
    if (!QX11Info::isCompositingManagerRunning()) {
        updateMask();
    }
    QDeclarativeView::resizeEvent(event);
}

void
DashDeclarativeView::updateMask()
{
    if (m_mode == FullScreenMode) {
        clearMask();
        return;
    }
    QBitmap bmp(size());
    {
        static QBitmap corner(unity2dDirectory() + "/places/artwork/desktop_dash_background_mask.png");
        static QBitmap top = corner.copy(0, 0, corner.width(), 1);
        static QBitmap left = corner.copy(0, 0, 1, corner.height());

        const int cornerX = bmp.width() - corner.width();
        const int cornerY = bmp.height() - corner.height();

        QPainter painter(&bmp);
        painter.fillRect(bmp.rect(), Qt::color1);
        painter.setBackgroundMode(Qt::OpaqueMode);
        painter.setBackground(Qt::color1);
        painter.setPen(Qt::color0);
        painter.drawPixmap(cornerX, cornerY, corner);

        painter.drawTiledPixmap(cornerX, 0, top.width(), cornerY, top);
        painter.drawTiledPixmap(0, cornerY, cornerX, left.height(), left);
    }
    setMask(bmp);
}
