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
#include <QFuture>
#include <QVector>

// X11
#include <X11/extensions/XInput.h>

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

    void getModifiers();
    bool registerEvents();
    void run();

    Display *m_display;
    QFuture<void> m_future;
    bool m_stop;
    QVector<XEventClass> m_eventList;
    QVector<KeyCode> m_modList;
};

#endif // KEYMONITOR_H

