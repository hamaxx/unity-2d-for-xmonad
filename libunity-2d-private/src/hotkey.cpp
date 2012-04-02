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

#include "hotkey.h"

#include <QX11Info>
#include <QDebug>
#include <QKeySequence>

#include <X11/Xlib.h>
#include <X11/XKBlib.h>
#include <X11/extensions/XKB.h>
#include <X11/Xproto.h>

// XK_MISCELLANY is needed for XK_KP_* keysymdef definitions
#define XK_MISCELLANY
#include <X11/keysymdef.h>

#include <debug_p.h>

static int (*_x_old_errhandler)(Display *, XErrorEvent *);
static int _x_grabkey_errhandler(Display *display, XErrorEvent *event)
{
    Q_UNUSED(display);
    if ((event->error_code == BadAccess ||
         event->error_code == BadValue ||
         event->error_code== BadWindow) &&
         event->request_code == X_GrabKey) {
        UQ_WARNING << "Call to XGrabKey failed, this usually means some"
                      " other client already reserved the hotkey.";
    }

    return 0;
}

Hotkey::Hotkey(Qt::Key key, Qt::KeyboardModifiers modifiers, QObject *parent) :
    QObject(parent), m_connections(0),
    m_key(key), m_modifiers(modifiers),
    m_x11key(0), m_x11modifiers(0)
{
    translateModifiers(modifiers);

    if (modifiers.testFlag(Qt::KeypadModifier)) {
        /* Support 0..9 numpad keys only.
           If we ever need to support additional numpad keys, then this logic should be extended
           in the spirit of Qt's qkeymapper_x11.cpp.  */
        if (key >= Qt::Key_0 && key <= Qt::Key_9) {
            /* Please note that we don't set Mod2Mask (NumLock) modifier. It appears that Mod2 is reported
               when it's actually held during numkey key press.
            */
            m_x11key = XKeysymToKeycode(QX11Info::display(), XK_KP_9 - (Qt::Key_9 - key));
        } else {
            UQ_WARNING << "Can't map numpad keys other than 0..9";
        }
    } else {
        /* Translate the QT key to X11 keycode */

        /* QKeySequence can be used to translate a Qt::Key in a format that is
           understood by XStringToKeysym if the sequence is composed only by the key */
        QString keyString = QKeySequence(key).toString();
        KeySym keysym = XStringToKeysym(keyString.toLatin1().data());
        if (keysym == NoSymbol) {
            /* XStringToKeysym doesn’t work well with exotic characters (such as
              'É'). Note that this fallback code path looks much simpler but doesn’t
              work for special keys such as the function keys (e.g. F1), which is
              why the translation with XStringToKeysym is attempted first. */
            keysym = (ushort) key;
        }
        m_x11key = XKeysymToKeycode(QX11Info::display(), keysym);
        if (m_x11key == 0) {
            UQ_WARNING << "Could not get keycode for keysym" << keysym
                       << "(" << keyString << ")";
        }
    }
}

Hotkey::Hotkey(uint x11key, Qt::KeyboardModifiers modifiers, QObject *parent)
 : QObject(parent), m_connections(0),
   m_key(0), m_modifiers(modifiers),
   m_x11key(x11key), m_x11modifiers(0)
{
    translateModifiers(modifiers);
}

void
Hotkey::connectNotify(const char * signal)
{
    Q_UNUSED(signal);
    if (m_connections == 0) {
        UQ_DEBUG << "Grabbing hotkey" << QKeySequence(m_key | m_modifiers).toString();
        _x_old_errhandler = XSetErrorHandler(_x_grabkey_errhandler);
        XGrabKey(QX11Info::display(), m_x11key, m_x11modifiers,
                 QX11Info::appRootWindow(), True, GrabModeAsync, GrabModeAsync);
        XSync(QX11Info::display(), False);
        XSetErrorHandler(_x_old_errhandler);
    }
    m_connections++;
}

void
Hotkey::disconnectNotify(const char * signal)
{
    Q_UNUSED(signal);
    if (m_connections == 1) {
        UQ_DEBUG << "Ungrabbing hotkey" << QKeySequence(m_key | m_modifiers).toString();
        XUngrabKey(QX11Info::display(), m_x11key, m_x11modifiers,
                   QX11Info::appRootWindow());
    }
    m_connections--;
}

bool
Hotkey::processNativeEvent(uint x11Keycode, uint x11Modifiers, bool isPressEvent)
{
    if (x11Keycode == m_x11key && x11Modifiers == m_x11modifiers) {
        Q_EMIT (isPressEvent) ? pressed() : released();
        return true;
    }
    return false;
}

void
Hotkey::translateModifiers(Qt::KeyboardModifiers modifiers)
{
    /* Translate the QT modifiers to X11 modifiers */
    if (modifiers.testFlag(Qt::ShiftModifier)) {
        m_x11modifiers |= ShiftMask ;
    }
    if (modifiers.testFlag(Qt::ControlModifier)) {
        m_x11modifiers |= ControlMask ;
    }
    if (modifiers.testFlag(Qt::AltModifier)) {
        m_x11modifiers |= Mod1Mask;
    }
    if (modifiers.testFlag(Qt::MetaModifier)) {
        m_x11modifiers |= Mod4Mask;
    }
}

#include "hotkey.moc"
