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

#ifndef HotkeyMonitor_H
#define HotkeyMonitor_H

#include <QObject>
#include <QList>

class Hotkey;

class HotkeyMonitor : public QObject
{
    Q_OBJECT

public:
    static HotkeyMonitor& instance();
    ~HotkeyMonitor();

    Hotkey* getHotkeyFor(Qt::Key key, Qt::KeyboardModifiers modifiers);

private:
    HotkeyMonitor(QObject* parent=0);

    static bool keyEventFilter(void* message);
    void processKeyEvent(uint x11Keycode, uint x11Modifiers,
                         bool isPressEvent);

    QList<Hotkey*> m_hotkeys;
};


#endif // HotkeyMonitor_H

