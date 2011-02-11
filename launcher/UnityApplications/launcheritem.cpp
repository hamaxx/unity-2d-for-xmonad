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
#include "dragdropevent.h"

LauncherItem::LauncherItem(QObject* parent): QObject(parent)
{
    m_menu = new LauncherContextualMenu;
    m_menu->setLauncherItem(this);
}

LauncherItem::~LauncherItem()
{
    delete m_menu;
}

QObject*
LauncherItem::menu() const
{
    return m_menu;
}

void
LauncherItem::onDragEnter(QObject* event)
{
    Q_UNUSED(event)
    /* Default to doing nothing, i.e. refusing the event. */
}

void
LauncherItem::onDragLeave(QObject* event)
{
    Q_UNUSED(event)
    /* Default to doing nothing. */
}

void
LauncherItem::onDrop(QObject* event)
{
    Q_UNUSED(event)
    /* Default to doing nothing. */
}

