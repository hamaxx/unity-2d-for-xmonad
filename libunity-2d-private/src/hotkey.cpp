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

Hotkey::Hotkey(Qt::Key key, Qt::KeyboardModifiers modifiers, QObject *parent) :
    QObject(parent), m_connections(0),
    m_key(key), m_modifiers(modifiers),
    m_x11key(0), m_x11modifiers(0)
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

    /* Translate the QT key to X11 keycode */

    /* QKeySequence can be used to translate a Qt::Key in a format that is
       understood by XStringToKeysym if the sequence is composed only by the key */
    QString keyString = QKeySequence(key).toString();
    KeySym keysym = XStringToKeysym(keyString.toLatin1().data());
    if (keysym == NoSymbol) {
        qWarning() << "Could not convert" << keyString << "to an x11 keysym";
    } else {
        m_x11key = XKeysymToKeycode(QX11Info::display(), keysym);
        if (m_x11key == 0) {
            qWarning() << "Could not get keycode for keysym" << keysym
                       << "(" << keyString << ")";
        }
    }
}

void
Hotkey::connectNotify(const char * signal)
{
    Q_UNUSED(signal);
    if (m_connections == 0) {
        qDebug() << "Grabbing hotkey" << QKeySequence(m_key | m_modifiers).toString();
        XGrabKey(QX11Info::display(), m_x11key, m_x11modifiers,
                 QX11Info::appRootWindow(), True, GrabModeAsync, GrabModeAsync);
    }
    m_connections++;
}

void
Hotkey::disconnectNotify(const char * signal)
{
    Q_UNUSED(signal);
    if (m_connections == 1) {
        qDebug() << "Ungrabbing hotkey" << QKeySequence(m_key | m_modifiers).toString();
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

#include "hotkey.moc"
