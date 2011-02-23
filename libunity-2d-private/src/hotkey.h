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

#ifndef Hotkey_H
#define Hotkey_H

#include <QObject>

class Hotkey : public QObject
{
    friend class HotkeyMonitor;

    Q_OBJECT
    Q_PROPERTY(Qt::Key key READ key NOTIFY keyChanged)
    Q_PROPERTY(Qt::KeyboardModifiers modifiers READ modifiers NOTIFY modifiersChanged)

public:
    Qt::Key key() const { return m_key; }
    Qt::KeyboardModifiers modifiers() const { return m_modifiers; }

Q_SIGNALS:
    void keyChanged(Qt::Key key);
    void modifiersChanged(Qt::KeyboardModifiers modifiers);
    void pressed();
    void released();

protected:
    virtual void connectNotify(const char * signal);
    virtual void disconnectNotify(const char * signal);

private:
    Hotkey(Qt::Key key, Qt::KeyboardModifiers modifiers, QObject *parent);
    bool processNativeEvent(uint x11Keycode, uint x11Modifiers, bool isPressEvent);

private:
    uint m_connections;
    Qt::Key m_key;
    Qt::KeyboardModifiers m_modifiers;
    uint m_x11key;
    uint m_x11modifiers;
};

#endif // Hotkey_H
