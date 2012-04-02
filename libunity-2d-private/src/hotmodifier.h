/*
 * Copyright (C) 2012 Canonical, Ltd.
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

#ifndef HOTMODIFIER_H
#define HOTMODIFIER_H

// Qt
#include <QObject>
#include <QTimer>

class HotModifier : public QObject
{
    Q_OBJECT
    Q_PROPERTY(Qt::KeyboardModifiers modifiers READ modifiers)
    Q_PROPERTY(bool held READ held NOTIFY heldChanged)

public:
    HotModifier(Qt::KeyboardModifiers modifiers, QObject *parent = 0);

    void onModifiersChanged(Qt::KeyboardModifiers modifiers);
    void disable();

    // getters
    bool held() const;
    Qt::KeyboardModifiers modifiers() const;

Q_SIGNALS:
    void tapped();
    void heldChanged(bool modifierHeld);

private Q_SLOTS:
    void ignoreCurrentPress();
    void updateHoldState();

private:
    QTimer m_holdTimer;
    Qt::KeyboardModifiers m_modifiers;
    bool m_pressed;
    bool m_partiallyPressed;
    bool m_held;
    bool m_ignored;
    bool m_otherModifierPressed;
};

#endif // HOTMODIFIER_H
