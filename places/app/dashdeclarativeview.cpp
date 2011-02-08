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
#include <QCloseEvent>
#include <QDeclarativeContext>
#include <QX11Info>
#include <QGraphicsObject>

#include <QDebug>

#include <X11/Xlib.h>
#include <X11/Xatom.h>

DashDeclarativeView::DashDeclarativeView()
: QDeclarativeView()
, m_state(HiddenDash)
{
    QDesktopWidget* desktop = QApplication::desktop();
    connect(desktop, SIGNAL(resized(int)), SIGNAL(screenGeometryChanged()));
    connect(desktop, SIGNAL(workAreaResized(int)), SIGNAL(availableGeometryChanged()));
}

void
DashDeclarativeView::fitToAvailableSpace(int screen)
{
    QDesktopWidget *desktop = QApplication::desktop();    
    int current_screen = desktop->screenNumber(this);

    if(screen == current_screen)
    {
        QRect geometry = desktop->availableGeometry(this);
        setGeometry(geometry);
        setFixedSize(geometry.size());
    }
}

void
DashDeclarativeView::focusOutEvent(QFocusEvent* event)
{
    QDeclarativeView::focusOutEvent(event);
    setActive(false);
}

void
DashDeclarativeView::setActive(bool value)
{
    if (value != active()) {
        if (value) {
            setDashState(FullScreenDash);
        } else {
            setDashState(HiddenDash);
        }
    }
}

bool
DashDeclarativeView::active() const
{
    return m_state != HiddenDash;
}

void
DashDeclarativeView::setDashState(DashDeclarativeView::DashState state)
{
    if (m_state == state) {
        return;
    }

    m_state = state;
    switch (state) {
    case CollapsedDesktopDash:
    case ExpandedDesktopDash:
        // Fall-through for now

    case FullScreenDash:
        show();
        raise();
        activateWindow();
        forceActivateWindow();
        emit activeChanged(true);
        break;

    case HiddenDash:
        hide();
        emit activeChanged(false);
        break;
    }
    emit dashStateChanged(m_state);
}

DashDeclarativeView::DashState
DashDeclarativeView::dashState() const
{
    return m_state;
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
    setActive(true);
    QMetaObject::invokeMethod(dash, "activatePlaceEntry", Qt::AutoConnection,
                              Q_ARG(QVariant, QVariant::fromValue(file)),
                              Q_ARG(QVariant, QVariant::fromValue(entry)),
                              Q_ARG(QVariant, QVariant::fromValue(section)));
}

void
DashDeclarativeView::activateHome()
{
    QGraphicsObject* dash = rootObject();
    setActive(true);
    QMetaObject::invokeMethod(dash, "activateHome", Qt::AutoConnection);
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
