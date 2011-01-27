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

class KeyMonitor : public QObject
{
    Q_OBJECT

public:
    KeyMonitor(int keycode, QObject* parent=0);
    ~KeyMonitor();

    void grabKey();
    void ungrabKey();

private:
    int m_keycode;
    bool m_grabbed;
};

#endif // KeyMonitor_H

