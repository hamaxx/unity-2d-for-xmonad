/*
 * Copyright (C) 2011 Canonical, Ltd.
 *
 * Authors:
 *  Olivier Tilloy <olivier.tilloy@canonical.com>
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

#include "keymonitor.h"

#include <X11/Xlib.h>
#include <X11/Xproto.h>

#include <QX11Info>

KeyMonitor::KeyMonitor(int keycode, QObject* parent)
    : QObject(parent)
    , m_keycode(keycode)
    , m_grabbed(false)
{
}

KeyMonitor::~KeyMonitor()
{
    if (m_grabbed) {
        ungrabKey();
    }
}

static bool xerror = false;

static int (*_original_x_errhandler)(Display* display, XErrorEvent* event);

static int _x_errhandler(Display* display, XErrorEvent* event)
{
    Q_UNUSED(display);
    switch (event->error_code) {
    case BadAccess:
    case BadValue:
    case BadWindow:
        if (event->request_code == X_GrabKey ||
            event->request_code == X_UngrabKey) {
            xerror  = true;
        }
    default:
        return 0;
    }
}

void
KeyMonitor::grabKey()
{
    Display* display = QX11Info::display();
    Window window = QX11Info::appRootWindow();
    Bool owner = True;
    int pointer = GrabModeAsync;
    int keyboard = GrabModeAsync;
    xerror = false;
    _original_x_errhandler = XSetErrorHandler(_x_errhandler);
    XGrabKey(display, m_keycode, 0, window, owner, pointer, keyboard);
    XSetErrorHandler(_original_x_errhandler);
    if (!xerror) {
        m_grabbed = true;
    }
}

void
KeyMonitor::ungrabKey()
{
    Display* display = QX11Info::display();
    Window window = QX11Info::appRootWindow();
    xerror = false;
    _original_x_errhandler = XSetErrorHandler(_x_errhandler);
    XUngrabKey(display, m_keycode, 0, window);
    XSetErrorHandler(_original_x_errhandler);
    if (!xerror) {
        m_grabbed = false;
    }
}

