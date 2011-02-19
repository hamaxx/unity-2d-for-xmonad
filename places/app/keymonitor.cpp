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

#include <QDebug>

#include <X11/X.h>
#include <X11/Xlib.h>

#include <QX11Info>
#include <QAbstractEventDispatcher>


//            DashDeclarativeView* view = getView();
//            if (view->active()) {
//                view->setActive(false);
//            }
//            else {
//                view->activateHome();
//}

static int (*original_x_errhandler)(Display* display, XErrorEvent* event);

static int qxt_x_errhandler(Display* display, XErrorEvent *event)
{
    Q_UNUSED(display);
    switch (event->error_code)
    {
        case BadAccess:
        case BadValue:
        case BadWindow:
            if (event->request_code == 33 /* X_GrabKey */ ||
                event->request_code == 34 /* X_UngrabKey */)
            {
                char errstr[256];
                XGetErrorText(QX11Info::display(), event->error_code, errstr, 256);
                qDebug() << "GrabKey failed:" << errstr;
            }
        default:
            return 0;
    }
}


Hotkey::Hotkey(QObject *parent, uint keycode, Qt::KeyboardModifiers modifiers) :
               QObject(parent), m_keycode(keycode),
               m_modifiers(modifiers), m_connections(0)
{
}

Hotkey::~Hotkey()
{
    XUngrabKey(QX11Info::display(),
               m_keycode, Hotkey::QtToX11Modifiers(m_modifiers),
               QX11Info::appRootWindow());
    /* FIXME: Assume no X error. */
}

void Hotkey::connectNotify(const char * signal)
{
    Q_UNUSED(signal);
    qDebug() << "CONNECTED" << m_connections;
    if (m_connections == 0) {
        qDebug() << "GRABBING" << m_keycode << Hotkey::QtToX11Modifiers(m_modifiers) << ControlMask;
        original_x_errhandler = XSetErrorHandler(qxt_x_errhandler);
        XGrabKey(QX11Info::display(),
                 m_keycode, Hotkey::QtToX11Modifiers(m_modifiers),
                 QX11Info::appRootWindow(), True,
                 GrabModeAsync, GrabModeAsync);
        XSync(QX11Info::display(), False);
        XSetErrorHandler(original_x_errhandler);
    }
    m_connections++;
}

void Hotkey::disconnectNotify(const char * signal)
{
    Q_UNUSED(signal);
    if (m_connections == 1) {
        XUngrabKey(QX11Info::display(),
                   m_keycode, Hotkey::QtToX11Modifiers(m_modifiers),
                   QX11Info::appRootWindow());
        /* FIXME: Assume no X error. */
    }
    m_connections--;
}


uint Hotkey::QtToX11Modifiers(Qt::KeyboardModifiers modifiers)
{
    uint x11Modifiers = 0;
    if (modifiers & Qt::ShiftModifier) {
        x11Modifiers |= ShiftMask;
    }
    if (modifiers & Qt::ControlModifier) {
        x11Modifiers |= ControlMask;
    }
    if (modifiers & Qt::AltModifier) {
        x11Modifiers |= Mod1Mask;
    }
    if (modifiers & Qt::MetaModifier) {
        x11Modifiers |= Mod4Mask;
    }
    return x11Modifiers;
}

Qt::KeyboardModifiers Hotkey::X11ToQtModifiers(uint modifiers)
{
    Qt::KeyboardModifiers qtModifiers = Qt::NoModifier;
    if (modifiers & ShiftMask) {
        qtModifiers |= Qt::ShiftModifier;
    }
    if (modifiers & ControlMask) {
        qtModifiers |= Qt::ControlModifier;
    }
    if (modifiers & Mod1Mask) {
        qtModifiers |= Qt::AltModifier;
    }
    if (modifiers & Mod4Mask) {
        qtModifiers |= Qt::MetaModifier;
    }
    return qtModifiers;
}


HotkeyMonitor::HotkeyMonitor(QObject* parent)
    : QObject(parent)
{
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

Hotkey* HotkeyMonitor::findHotkey(uint keycode, Qt::KeyboardModifiers modifiers)
{
    Q_FOREACH(Hotkey* hotkey, m_hotkeys) {
        if (hotkey->keycode() == keycode && hotkey->modifiers() == modifiers) {
            return hotkey;
        }
    }
    return NULL;
}

/* TODO: Use keysyms instead ? like:
   XKeysymToKeycode(display, XStringToKeysym(QKeySequence(key).toString().toLatin1().data())) */
Hotkey*
HotkeyMonitor::hotkey(uint keycode, Qt::KeyboardModifiers modifiers)
{
    Hotkey *hotkey = findHotkey(keycode, modifiers);
    if (hotkey == NULL) {
        hotkey = new Hotkey(this, keycode, modifiers);
        m_hotkeys.append(hotkey);
    }

    return hotkey;
}

bool
HotkeyMonitor::keyEventFilter(void* message)
{
    XEvent* event = static_cast<XEvent*>(message);
    if (event->type == KeyRelease)
    {
        XKeyEvent* key = (XKeyEvent*) event;
        HotkeyMonitor::instance().processKeyEvent(key->keycode, key->state);
    }
    return false;
}

void
HotkeyMonitor::processKeyEvent(uint keycode, uint modifiers)
{
    qDebug() << keycode << modifiers << Hotkey::X11ToQtModifiers(modifiers);
}

