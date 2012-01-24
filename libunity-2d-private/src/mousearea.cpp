/*
 * This file is part of unity-2d
 *
 * Copyright 2011 Canonical Ltd.
 *
 * Authors:
 * - Aurélien Gâteau <aurelien.gateau@canonical.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published
 * by the Free Software Foundation; version 3.
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
#include "mousearea.h"

// Local
#include <debug_p.h>

// Qt
#include <QX11Info>

// X11
#include <X11/Xlib.h>

struct MouseAreaPrivate
{
    Window m_window;
    QRect m_geometry;
    bool m_containsMouse;
};

MouseArea::MouseArea(QObject* parent)
: QObject(parent)
, d(new MouseAreaPrivate)
{
    Unity2dApplication* application = Unity2dApplication::instance();
    if (application == NULL) {
        /* This can happen for example when using qmlviewer */
        UQ_WARNING << "The application is not an Unity2dApplication."
                      "MouseArea disabled.";
    } else {
        application->installX11EventFilter(this);
    }

    d->m_containsMouse = false;

    XSetWindowAttributes attributes;
    attributes.override_redirect = True;
    attributes.event_mask = EnterWindowMask | LeaveWindowMask;
    unsigned long attributesMask = CWOverrideRedirect | CWEventMask;

    d->m_geometry = QRect(0, 0, 1, 1);

    d->m_window = XCreateWindow(QX11Info::display(), QX11Info::appRootWindow(),
        0, 0, 1, 1, // geometry
        0, // border_width
        0, // depth
        InputOnly, // class
        CopyFromParent, // visual
        attributesMask,
        &attributes);

    XMapWindow(QX11Info::display(), d->m_window);
}

MouseArea::~MouseArea()
{
    XDestroyWindow(QX11Info::display(), d->m_window);
    delete d;
}

QRect MouseArea::geometry() const
{
    return d->m_geometry;
}

void MouseArea::setGeometry(const QRect& rect)
{
    setGeometry(rect.x(), rect.y(), rect.width(), rect.height());
}

void MouseArea::setGeometry(int x, int y, int width, int height)
{
    d->m_geometry = QRect(x, y, width, height);
    XMoveResizeWindow(QX11Info::display(), d->m_window, x, y, width, height);
}

bool MouseArea::x11EventFilter(XEvent* _event)
{
    if (_event->type == EnterNotify || _event->type == LeaveNotify) {
        XCrossingEvent* event = (XCrossingEvent*)(_event);
        if (event->window == d->m_window) {
            d->m_containsMouse = event->type == EnterNotify;
            if (d->m_containsMouse) {
                entered();
            } else {
                exited();
            }
        }
    }
    return false;
}

bool MouseArea::containsMouse() const
{
    return d->m_containsMouse;
}

#include "mousearea.moc"
