/*
 * This file is part of unity-2d
 *
 * Copyright 2011 Canonical Ltd.
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
#include "keyboardmodifiersmonitor.h"

// Local
#include <debug_p.h>

// Qt
#include <QX11Info>

// X11
#include <X11/Xlib.h>
#include <X11/XKBlib.h>
#include <X11/extensions/XKB.h>

static int sXkbBaseEventType = 0;

static void setupXkb()
{
    int opcode, error;
    XkbQueryExtension(QX11Info::display(), &opcode, &sXkbBaseEventType,  &error, NULL, NULL);
    XkbSelectEvents(QX11Info::display(), XkbUseCoreKbd, XkbStateNotifyMask, XkbStateNotifyMask);
}

struct KeyboardModifiersMonitorPrivate
{
    KeyboardModifiersMonitorPrivate()
    : m_modifiers(0)
    {}

    int m_modifiers;
};

KeyboardModifiersMonitor::KeyboardModifiersMonitor(QObject *parent)
: QObject(parent)
, d(new KeyboardModifiersMonitorPrivate)
{
    if (sXkbBaseEventType == 0) {
        setupXkb();
    }

    Unity2dApplication* application = Unity2dApplication::instance();
    if (application == NULL) {
        /* This can happen for example when using qmlviewer to run the launcher */
        UQ_WARNING << "The application is not an Unity2dApplication."
                      "Modifiers will not be monitored.";
    } else {
        application->installX11EventFilter(this);
    }
}

KeyboardModifiersMonitor::~KeyboardModifiersMonitor()
{
    delete d;
}

KeyboardModifiersMonitor* KeyboardModifiersMonitor::instance()
{
    static KeyboardModifiersMonitor* monitor = new KeyboardModifiersMonitor();
    return monitor;
}

bool KeyboardModifiersMonitor::x11EventFilter(XEvent* event)
{
    if (event->type == sXkbBaseEventType + XkbEventCode) {
        XkbEvent *xkbEvent = (XkbEvent*)event;
        if (xkbEvent->any.xkb_type == XkbStateNotify) {
            d->m_modifiers = xkbEvent->state.mods;
            keyboardModifiersChanged(keyboardModifiers());
        }
    }
    return false;
}

Qt::KeyboardModifiers KeyboardModifiersMonitor::keyboardModifiers() const
{
    Qt::KeyboardModifiers value = 0;
    if (d->m_modifiers & ShiftMask) {
        value |= Qt::ShiftModifier;
    }
    if (d->m_modifiers & ControlMask) {
        value |= Qt::ControlModifier;
    }
    if (d->m_modifiers & Mod1Mask) {
        value |= Qt::AltModifier;
    }
    if (d->m_modifiers & Mod4Mask) {
        value |= Qt::MetaModifier;
    }
    return value;
}

#include "keyboardmodifiersmonitor.moc"
