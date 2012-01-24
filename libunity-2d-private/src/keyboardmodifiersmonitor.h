/*
 * This file is part of unity-2d
 *
 * Copyright 2011 Canonical Ltd.
 *
 * Authors:
 * - Aurélien Gâteau <aurelien.gateau@canonical.com>
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

#ifndef KEYBOARDMODIFIERMONITOR_H
#define KEYBOARDMODIFIERMONITOR_H

// Local
#include <unity2dapplication.h>

// Qt

struct KeyboardModifiersMonitorPrivate;

/**
 * This class monitor keyboard modifiers. It is able to track changes even if
 * the active window does not belong to the application.
 *
 * You *must* use Unity2dApplication to be able to use this class.
 *
 * In most case you don't need your own instance: use the one returned by
 * instance() instead.
 */
class KeyboardModifiersMonitor : public QObject, protected AbstractX11EventFilter
{
Q_OBJECT
public:
    KeyboardModifiersMonitor(QObject *parent = 0);
    ~KeyboardModifiersMonitor();

    Qt::KeyboardModifiers keyboardModifiers() const;

    static KeyboardModifiersMonitor* instance();

Q_SIGNALS:
    void keyboardModifiersChanged(Qt::KeyboardModifiers);

protected:
    bool x11EventFilter(XEvent*);

private:
    KeyboardModifiersMonitorPrivate* const d;
};

#endif /* KEYBOARDMODIFIERMONITOR_H */
