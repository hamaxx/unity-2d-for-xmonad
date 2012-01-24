/*
 * Copyright (C) 2010 Canonical, Ltd.
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

#include "launcheritem.h"
#include "launchermenu.h"

// libunity-2d
#include <hotkey.h>
#include <hotkeymonitor.h>

LauncherItem::LauncherItem(QObject* parent)
    : QObject(parent)
    , m_shortcutKey((Qt::Key) 0)
{
    m_menu = new LauncherContextualMenu;
    m_menu->setLauncherItem(this);
}

LauncherItem::~LauncherItem()
{
    delete m_menu;
}

Qt::Key
LauncherItem::shortcutKey() const
{
    return m_shortcutKey;
}

void
LauncherItem::setShortcutKey(Qt::Key key)
{
    if (m_shortcutKey != key) {
        if (m_shortcutKey != 0) {
            Hotkey* hotkey = HotkeyMonitor::instance().getHotkeyFor(m_shortcutKey, Qt::MetaModifier);
            disconnect(hotkey, SIGNAL(pressed()), this, SLOT(activate()));
        }
        m_shortcutKey = key;
        if (m_shortcutKey != 0) {
            Hotkey* hotkey = HotkeyMonitor::instance().getHotkeyFor(m_shortcutKey, Qt::MetaModifier);
            connect(hotkey, SIGNAL(pressed()), SLOT(activate()));
        }
        Q_EMIT shortcutKeyChanged(m_shortcutKey);
    }
}

QObject*
LauncherItem::menu() const
{
    return m_menu;
}

void
LauncherItem::onDragEnter(DeclarativeDragDropEvent* event)
{
    Q_UNUSED(event)
    /* Default to doing nothing, i.e. refusing the event. */
}

void
LauncherItem::onDrop(DeclarativeDragDropEvent* event)
{
    Q_UNUSED(event)
    /* Default to doing nothing. */
}

void
LauncherItem::launchNewInstance()
{
    /* Default to doing nothing. */
}

bool
LauncherItem::progressBarVisible() const
{
    return false;
}

float
LauncherItem::progress() const
{
    return 0.0;
}

bool
LauncherItem::counterVisible() const
{
    return false;
}

int
LauncherItem::counter() const
{
    return 0;
}

bool
LauncherItem::emblemVisible() const
{
    return false;
}

QString
LauncherItem::emblem() const
{
    return QString();
}

#include "launcheritem.moc"
