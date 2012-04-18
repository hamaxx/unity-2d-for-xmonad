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

class HotModifier;

/**
 * This class monitors global keypresses. Whenever a non-modifier is pressed,
 * keyPressed() is emitted.
 */
class KeyMonitor : public QObject
{
    Q_OBJECT

public:
    static KeyMonitor* instance();
    ~KeyMonitor();

    Qt::KeyboardModifiers keyboardModifiers() const;

    HotModifier* getHotModifierFor(Qt::KeyboardModifiers modifiers);

    void disableModifiers(Qt::KeyboardModifiers modifiers);
    void enableModifiers(Qt::KeyboardModifiers modifiers);

Q_SIGNALS:
    void keyPressed();
    void keyboardModifiersChanged(Qt::KeyboardModifiers);

private:
    KeyMonitor(QObject* parent=0);

    void getModifiers();
    bool registerEvents();

private Q_SLOTS:
    void x11EventDispatch();

private:
    Display *m_display;
    QVector<XEventClass> m_eventList;
    QVector<KeyCode> m_modList;
    Qt::KeyboardModifiers m_modifiers;
    QList<HotModifier*> m_hotModifiers;
    Qt::KeyboardModifiers m_disabledModifiers;
};

#endif // KEYMONITOR_H

