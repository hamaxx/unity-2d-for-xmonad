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

#include <debug_p.h>

HotkeyMonitor::HotkeyMonitor(QObject* parent)
    : QObject(parent)
{
    int opcode, baseError, baseEvent;
    if (XkbQueryExtension(QX11Info::display(), &opcode, &baseEvent,
                          &baseError, NULL, NULL) == False) {
        UQ_WARNING << "Failed to initialize Xkb extension. CapsLock and NumLock"
                      "active will prevent shortcuts from working.";
    } else {
        /* With this call we ignore CapsLock and NumLock when grabbing keys. */
        XkbSetIgnoreLockMods(QX11Info::display(), XkbUseCoreKbd,
                             Mod2Mask | LockMask, Mod2Mask | LockMask,
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
}

#include "hotkeymonitor.moc"
