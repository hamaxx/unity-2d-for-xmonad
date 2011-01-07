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

#ifndef SPREADVIEW_H
#define SPREADVIEW_H

#include <QDeclarativeView>
#include <QDebug>
#include <QShowEvent>

#include <QX11Info>
#include <X11/Xlib.h>


class SpreadView : public QDeclarativeView
{
    Q_OBJECT

public slots:
    /* FIXME: copied from places/app/dashdeclarativeview.h */
    void fitToAvailableSpace(int screen);

    /* FIXME: copied from places/app/dashdeclarativeview.h */
    void forceActivateWindow();

    Q_INVOKABLE void grabAll() {
        qDebug() << "Visible:" << this->isVisible();

        XWindowAttributes attr;
        XGetWindowAttributes(QX11Info::display(), this->winId(), &attr);
        qDebug() << "Viewable X11:" << ((attr.map_state == IsViewable) ? "IsViewable" :
                                       (attr.map_state == IsUnmapped) ? "IsUnmapped" :
                                       (attr.map_state == IsUnviewable) ? "IsUnviewable" : "<?>");

        if (mouseGrabber()) mouseGrabber()->releaseMouse();
        int status = XGrabPointer(QX11Info::display(), this->winId(), False,
                      (uint)(ButtonPressMask | ButtonReleaseMask |
                             PointerMotionMask | EnterWindowMask | LeaveWindowMask),
                      GrabModeAsync, GrabModeAsync,
                      None, None, CurrentTime);
        const char *s =
        status == GrabNotViewable ? "\"GrabNotViewable\"" :
        status == AlreadyGrabbed  ? "\"AlreadyGrabbed\"" :
        status == GrabFrozen      ? "\"GrabFrozen\"" :
        status == GrabInvalidTime ? "\"GrabInvalidTime\"" :
        "<?>";

        qDebug() << "Grabbing mouse status:" << status << "(" << s << ")";

        qDebug() << "Mouse grabber: " << mouseGrabber();
        qDebug() << "Mouse grabber: " << (this == mouseGrabber());

        grabKeyboard();
        qDebug() << "Keyb grabber: " << keyboardGrabber();
        qDebug() << "Keyb grabber: " << (this == keyboardGrabber());
    }

protected:
    virtual void showEvent ( QShowEvent * event ) {
        qDebug() << "++++++++++++++++++ ShowEvent. spontaneous:" << event->spontaneous();
        grabAll();
    }

public:
    explicit SpreadView();
};

#endif // SPREADVIEW_H

