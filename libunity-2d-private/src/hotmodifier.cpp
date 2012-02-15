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

// Self
#include "hotmodifier.h"

// Unity-2D
#include "keymonitor.h"

static const int KEY_HOLD_THRESHOLD = 250;

HotModifier::HotModifier(Qt::KeyboardModifiers modifiers, QObject *parent) :
    QObject(parent)
,   m_modifiers(modifiers)
,   m_pressed(false)
,   m_held(false)
,   m_ignored(false)
{
    m_holdTimer.setSingleShot(true);
    m_holdTimer.setInterval(KEY_HOLD_THRESHOLD);
    connect(&m_holdTimer, SIGNAL(timeout()), SLOT(updateHoldState()));

    KeyMonitor* keymonitor = KeyMonitor::instance();
    connect(keymonitor, SIGNAL(keyPressed()), SLOT(ignoreCurrentPress()));
}

Qt::KeyboardModifiers
HotModifier::modifiers() const
{
    return m_modifiers;
}

bool
HotModifier::held() const
{
    return m_held;
}

void
HotModifier::onModifiersChanged(Qt::KeyboardModifiers modifiers)
{
    bool pressed = m_modifiers == modifiers;
    if (!m_ignored && m_pressed && !m_held && !pressed) {
        Q_EMIT tapped();
    } else if (m_pressed && m_held && !pressed) {
        m_held = false;
        Q_EMIT heldChanged(m_held);
    }
    if (!m_pressed && pressed) {
        m_ignored = false;
        m_holdTimer.start();
    }
    m_pressed = pressed;
}

void
HotModifier::updateHoldState()
{
    /* If the key was released in the meantime, just do nothing, otherwise
       consider the key being held, unless we're told to ignore it. */
    if (m_pressed && !m_ignored) {
        m_held = true;
        Q_EMIT heldChanged(m_held);
    }
}

void
HotModifier::ignoreCurrentPress()
{
    if (m_pressed) {
        m_ignored = true;
    }
}

void
HotModifier::disable()
{
    m_holdTimer.stop();
    m_pressed = false;
    m_ignored = false;
    if (m_held) {
        m_held = false;
        Q_EMIT heldChanged(false);
    }
}

#include "hotmodifier.moc"
