/*
 * Copyright (C) 2011 Canonical, Ltd.
 *
 * Authors:
 *  Michał Sawicz <michal.sawicz@canonical.com>
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

#ifndef KEYMONITOR_H
#define KEYMONITOR_H

// Qt
#include <QObject>

struct KeyMonitorPrivate;

/**
 * This class monitors global keypresses. Whenever a non-modifier is pressed,
 * keyPressed() is emitted.
 *
 * In most case you don't need your own instance: use the one returned by
 * instance() instead.
 */
class KeyMonitor : public QObject
{
    Q_OBJECT

public:
    static KeyMonitor* instance();
    ~KeyMonitor();

Q_SIGNALS:
    void keyPressed();

private:
    KeyMonitor(QObject* parent=0);

    bool register_events();
    void get_modifiers();
    void run();

    KeyMonitorPrivate* const d;
};

#endif // KEYMONITOR_H

