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

#ifndef KeyMonitor_H
#define KeyMonitor_H

#include <QObject>
#include <QList>
#include <QPair>

class HotkeyMonitor;

class Hotkey : public QObject
{
    friend class HotkeyMonitor;

    Q_OBJECT
    Q_PROPERTY(uint keycode READ keycode)
    Q_PROPERTY(Qt::KeyboardModifiers modifiers READ modifiers)

public:
    ~Hotkey();
    uint keycode() const { return m_keycode; }
    Qt::KeyboardModifiers modifiers() const { return m_modifiers; }

    static uint QtToX11Modifiers(Qt::KeyboardModifiers modifiers);
    static Qt::KeyboardModifiers X11ToQtModifiers(uint modifiers);

Q_SIGNALS:
    void activated();

protected:
    virtual void connectNotify(const char * signal);
    virtual void disconnectNotify(const char * signal);

private:
    Hotkey(QObject *parent = 0, uint keycode = 0,
           Qt::KeyboardModifiers modifiers = Qt::NoModifier);

    uint m_keycode;
    Qt::KeyboardModifiers m_modifiers;
    uint m_connections;
    static bool x11error;
};


class HotkeyMonitor : public QObject
{
    Q_OBJECT

public:
    static HotkeyMonitor& instance();
    ~HotkeyMonitor();

    Hotkey* hotkey(uint keycode, Qt::KeyboardModifiers modifiers);

private:
    HotkeyMonitor(QObject* parent=0);

    static bool keyEventFilter(void* message);
    void processKeyEvent(uint keycode, uint modifiers);
    uint translateModifiers(Qt::KeyboardModifiers modifiers);
    Hotkey* findHotkey(uint keycode, Qt::KeyboardModifiers modifiers);

    QList<Hotkey*> m_hotkeys;
};


#endif // KeyMonitor_H

