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

void
KeyMonitor::grabKey()
{
    Display* display = QX11Info::display();
    Window window = QX11Info::appRootWindow();
    Bool owner = True;
    int pointer = GrabModeAsync;
    int keyboard = GrabModeAsync;
    XGrabKey(display, m_keycode, 0, window, owner, pointer, keyboard);
    /* Assume no X error. */
    m_grabbed = true;
}

void
KeyMonitor::ungrabKey()
{
    Display* display = QX11Info::display();
    Window window = QX11Info::appRootWindow();
    XUngrabKey(display, m_keycode, 0, window);
    /* Assume no X error. */
    m_grabbed = false;
}

