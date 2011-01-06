/*
 * This file is part of unity-qt
 *
 * Copyright 2011 Canonical Ltd.
 *
 * Authors:
 * - Aurélien Gâteau <aurelien.gateau@canonical.com>
 *
 * License: GPL v3
 */
// Self
#include "keyboardmodifiersmonitor.h"

// Local
#include "debug_p.h"

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
    XkbSelectEvents(QX11Info::display(), XkbUseCoreKbd, XkbAllEventsMask, XkbAllEventsMask);
}

struct KeyboardModifiersMonitorPrivate
{
    KeyboardModifiersMonitorPrivate()
    : m_modifiers(0)
    {}

    int m_modifiers;
};

KeyboardModifiersMonitor::KeyboardModifiersMonitor(QObject* parent)
: QObject(parent)
, d(new KeyboardModifiersMonitorPrivate)
{
    if (sXkbBaseEventType == 0) {
        setupXkb();
    }

    UnityQtApplication* app = UnityQtApplication::instance();
    Q_ASSERT(app);
    UnityQtApplication::instance()->installX11EventFilter(this);
}

KeyboardModifiersMonitor::~KeyboardModifiersMonitor()
{
    delete d;
}

KeyboardModifiersMonitor* KeyboardModifiersMonitor::instance()
{
    static KeyboardModifiersMonitor* monitor = new KeyboardModifiersMonitor(UnityQtApplication::instance());
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
