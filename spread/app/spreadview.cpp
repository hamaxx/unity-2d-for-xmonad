/*
 * Copyright (C) 2010 Canonical, Ltd.
 *
 * Authors:
 *  Ugo Riboni <ugo.riboni@canonical.com>
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

#include "spreadview.h"

#include <QDesktopWidget>
#include <QApplication>
#include <QMouseEvent>
#include <QX11Info>

#include <X11/Xlib.h>
#include <X11/Xatom.h>

SpreadView::SpreadView() : QDeclarativeView()
{
}

/* FIXME: copied from places/app/dashdeclarativeview.cpp */
void SpreadView::fitToAvailableSpace(int screen)
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

/* Since we are a dock window, the WM never gives us keyboard focus.
   The workaround below, copied from places, will fix this */
/* FIXME: copied from places/app/dashdeclarativeview.cpp */
void
SpreadView::forceActivateWindow()
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

/* To be able to call grabMouse() we need to be 100% sure that X11 did
   already map the window. Otherwise grabMouse() will silently fail (and
   confusingly incorrectly reports the widget to be the mouseGrabber())

   Grabbing the mouse just after calling show() on the wiew
   will not work, since the window is really not visible yet.

   One would expect showEvent to be a good candidate, but it doesn't
   work either. According to the docs of QWidget::showEvent the window is
   really shown only when the event's spontaneous() flag is true, however we
   never seem to receive showEvents with this flag set to true.

   The first time the window seems to be actually visible is in focusEvent.
*/
void SpreadView::focusInEvent(QFocusEvent * event)
{
    /* Note that we grab mouse input from the viewport because doing it directly
       in the view won't work.
       It appears that the mouse events are first handled by the vieweport
       and only afterwards by the view itself. If the view grabs all mouse
       input, then the viewport doesn't receive these events anymore and the
       mouse stops working.
       Keyboard events doesn't seem to go the same route and would be unaffected,
       but for consistency let's grab everything from the viewport anyway.
     */
//    this->viewport()->installEventFilter(this);
//    this->viewport()->grabKeyboard();
//    this->viewport()->grabMouse();

    QDeclarativeView::focusInEvent(event);
}

/* This is not strictly necessary as X11 will cancel any grabs when the
   window is unmapped, and put them back in place when it's mapped again, but
   let's do it anyway for the sake of consistency.
*/
void SpreadView::focusOutEvent(QFocusEvent * event)
{
    this->viewport()->releaseKeyboard();
    this->viewport()->releaseMouse();
    this->viewport()->removeEventFilter(this);

    QDeclarativeView::focusOutEvent(event);
}

bool SpreadView::eventFilter(QObject *obj, QEvent *event) {
    if (event->type() == QEvent::MouseButtonPress) {
        if (!this->viewport()->geometry().contains(((QMouseEvent*)event)->pos())) {
            emit outsideClick();
        }
    }

    return false;
}
