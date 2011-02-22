/*
 * Copyright (C) 2011 Canonical, Ltd.
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

#include "hotkeymonitor.h"
#include "hotkey.h"

#include <QDebug>

#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/XKBlib.h>
#include <X11/extensions/XKB.h>

#include <QX11Info>
#include <QAbstractEventDispatcher>

HotkeyMonitor::HotkeyMonitor(QObject* parent)
    : QObject(parent)
{
    int opcode, baseError, baseEvent;
    if (XkbQueryExtension(QX11Info::display(), &opcode, &baseEvent,  &baseError, NULL, NULL) == False) {
        qWarning() << "Failed to initialize the Xkb extension. Some hotkeys may not work.";
    } else {
        /* By calling this function we ask the xkb extension to make key grabs ignore the effect
           of any of the listed modifiers. This is useful to avoid modifiers as caps or num lock to
           affect more normal hotkey shortcuts like CTRL+F1 or similar. */
        XkbSetIgnoreLockMods(QX11Info::display(), XkbUseCoreKbd,
                             Mod2Mask | LockMask, Mod2Mask | LockMask, // NUMLOCK and CAPSLOCK
                             0, 0);
    }

    QAbstractEventDispatcher::instance()->setEventFilter(HotkeyMonitor::keyEventFilter);
}

HotkeyMonitor&
HotkeyMonitor::instance()
{
    static HotkeyMonitor monitor;
    return monitor;
}

HotkeyMonitor::~HotkeyMonitor()
{
    qDeleteAll(m_hotkeys);
}


Hotkey*
HotkeyMonitor::getHotkeyFor(Qt::Key key, Qt::KeyboardModifiers modifiers)
{
    Q_FOREACH(Hotkey* currentHotkey, m_hotkeys) {
        if (currentHotkey->key() == key &&
            currentHotkey->modifiers() == modifiers) {
            return currentHotkey;
        }
    }

    Hotkey *hotkey = new Hotkey(key, modifiers, this);
    m_hotkeys.append(hotkey);
    return hotkey;
}

bool
HotkeyMonitor::keyEventFilter(void* message)
{
    XEvent* event = static_cast<XEvent*>(message);
    if (event->type == KeyRelease || event->type == KeyPress)
    {
        XKeyEvent* key = (XKeyEvent*) event;
        HotkeyMonitor::instance().processKeyEvent(key->keycode, key->state,
                                                  event->type == KeyPress);
    }
    return false;
}

void
HotkeyMonitor::processKeyEvent(uint x11Keycode, uint x11Modifiers,
                               bool isPressEvent)
{
    Q_FOREACH(Hotkey* hotkey, m_hotkeys) {
        if (hotkey->processNativeEvent(x11Keycode, x11Modifiers, isPressEvent)) {
            return;
        }
    }
    qWarning() << "Received x11 key event that wasn't processed by any hotkey:"
               << x11Keycode << x11Modifiers << ((isPressEvent) ? "Press" : "Release");
}

#include "hotkeymonitor.moc"
