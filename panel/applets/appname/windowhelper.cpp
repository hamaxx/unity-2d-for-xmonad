/*
 * Plasma applet to display DBus global menu
 *
 * Copyright 2009 Canonical Ltd.
 *
 * Authors:
 * - Aurélien Gâteau <aurelien.gateau@canonical.com>
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

// Self
#include "windowhelper.h"

// Local

// unity-2d
#include <debug_p.h>
#include <gconnector.h>

// Bamf
#include <bamf-matcher.h>
#include <bamf-window.h>

// libwnck
extern "C" {
#define WNCK_I_KNOW_THIS_IS_UNSTABLE
#include <libwnck/libwnck.h>
}

// Qt
#include <QDateTime>
#include <QApplication>
#include <QDesktopWidget>
#include <QPoint>

// X11
#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <QX11Info>

struct WindowHelperPrivate
{
    WnckWindow* m_window;
    GConnector m_connector;
};

WindowHelper::WindowHelper(QObject* parent)
: QObject(parent)
, d(new WindowHelperPrivate)
{
    d->m_window = 0;

    WnckScreen* screen = wnck_screen_get_default();
    wnck_screen_force_update(screen);

    update();

    connect(&BamfMatcher::get_default(), SIGNAL(ActiveWindowChanged(BamfWindow*,BamfWindow*)),
        SLOT(update()));
    /* Work around a bug in BAMF: the ActiveWindowChanged signal is not emitted
       for some windows that open maximized. This is for example the case of the
       LibreOffice startcenter. */
    connect(&BamfMatcher::get_default(), SIGNAL(ViewOpened(BamfView*)),
        SLOT(update()));
    // Work around a BAMF bug: it does not emit ActiveWindowChanged when the
    // last window is closed. Should be removed when this bug is fixed.
    connect(&BamfMatcher::get_default(), SIGNAL(ViewClosed(BamfView*)),
        SLOT(update()));
}

WindowHelper::~WindowHelper()
{
    delete d;
}

static void stateChangedCB(GObject* window,
    WnckWindowState changed_mask,
    WnckWindowState new_state,
    WindowHelper*  watcher)
{
    QMetaObject::invokeMethod(watcher, "stateChanged");
}

static void nameChangedCB(GObject* window,
    WindowHelper*  watcher)
{
    QMetaObject::invokeMethod(watcher, "nameChanged");
}

void WindowHelper::update()
{
    BamfWindow* bamfWindow = BamfMatcher::get_default().active_window();
    uint xid = bamfWindow ? bamfWindow->xid() : 0;

    if (d->m_window) {
        d->m_connector.disconnectAll();
        d->m_window = 0;
    }
    if (xid != 0) {
        d->m_window = wnck_window_get(xid);
        d->m_connector.connect(G_OBJECT(d->m_window), "name-changed", G_CALLBACK(nameChangedCB), this);
        d->m_connector.connect(G_OBJECT(d->m_window), "state-changed", G_CALLBACK(stateChangedCB), this);
    }
    stateChanged();
    nameChanged();
}

bool WindowHelper::isMaximized() const
{
    if (!d->m_window) {
        return false;
    }
    return wnck_window_is_maximized(d->m_window);
}

bool WindowHelper::isMostlyOnScreen(int screen) const
{
    if (!d->m_window) {
        return false;
    }
    int x, y, width, height;
    wnck_window_get_geometry(d->m_window, &x, &y, &width, &height);
    const QRect windowGeometry(x, y, width, height);
    QDesktopWidget* desktop = QApplication::desktop();
    const QRect screenGeometry = desktop->screenGeometry(screen);
    QRect onScreen = screenGeometry.intersected(windowGeometry);
    int intersected = onScreen.width() * onScreen.height();
    for (int i = 0; i < desktop->screenCount(); ++i) {
        if (i != screen) {
            onScreen = desktop->screenGeometry(i).intersected(windowGeometry);
            if (onScreen.width() * onScreen.height() > intersected) {
                return false;
            }
        }
    }
    return true;
}

void WindowHelper::close()
{
    guint32 timestamp = QDateTime::currentDateTime().toTime_t();
    wnck_window_close(d->m_window, timestamp);
}

void WindowHelper::minimize()
{
    wnck_window_minimize(d->m_window);
}

void WindowHelper::unmaximize()
{
    wnck_window_unmaximize(d->m_window);
}

void WindowHelper::drag(const QPoint& pos)
{
    // this code IMO should ultimately belong to wnck
    if (wnck_window_is_maximized(d->m_window)) {
        XEvent xev;
        QX11Info info;
        Atom netMoveResize = XInternAtom(QX11Info::display(), "_NET_WM_MOVERESIZE", false);
        xev.xclient.type = ClientMessage;
        xev.xclient.message_type = netMoveResize;
        xev.xclient.display = QX11Info::display();
        xev.xclient.window = wnck_window_get_xid(d->m_window);
        xev.xclient.format = 32;
        xev.xclient.data.l[0] = pos.x();
        xev.xclient.data.l[1] = pos.y();
        xev.xclient.data.l[2] = 8; // _NET_WM_MOVERESIZE_MOVE
        xev.xclient.data.l[3] = Button1;
        xev.xclient.data.l[4] = 0;
        XUngrabPointer(QX11Info::display(), QX11Info::appTime());
        XSendEvent(QX11Info::display(), QX11Info::appRootWindow(info.screen()), false,
                   SubstructureRedirectMask | SubstructureNotifyMask, &xev);
    }
}

#include "windowhelper.moc"
