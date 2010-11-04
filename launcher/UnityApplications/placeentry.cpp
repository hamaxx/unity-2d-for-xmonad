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

#include "placeentry.h"

#include <QDBusInterface>

PlaceEntry::PlaceEntry()
{
}

PlaceEntry::PlaceEntry(const PlaceEntry& other) :
    m_fileName(other.m_fileName),
    m_groupName(other.m_groupName),
    m_dbusObjectPath(other.m_dbusObjectPath),
    m_icon(other.m_icon),
    m_name(other.m_name)
{
}

PlaceEntry::~PlaceEntry()
{
}

bool
PlaceEntry::active() const
{
    // TODO: implement me
    return false;
}

bool
PlaceEntry::running() const
{
    return false;
}

bool
PlaceEntry::urgent() const
{
    return false;
}

QString
PlaceEntry::name() const
{
    return m_name;
}

void
PlaceEntry::setName(QString name)
{
    m_name = name;
}

QString
PlaceEntry::icon() const
{
    return m_icon;
}

void
PlaceEntry::setIcon(QString icon)
{
    m_icon = icon;
}

bool
PlaceEntry::launching() const
{
    // This basically means no launching animation when opening the device.
    // Unity behaves likes this.
    return false;
}

QString
PlaceEntry::fileName() const
{
    return m_fileName;
}

void
PlaceEntry::setFileName(QString fileName)
{
    m_fileName = fileName;
}

QString
PlaceEntry::groupName() const
{
    return m_groupName;
}

void
PlaceEntry::setGroupName(QString groupName)
{
    m_groupName = groupName;
}

QString
PlaceEntry::dbusObjectPath() const
{
    return m_dbusObjectPath;
}

void
PlaceEntry::setDbusObjectPath(QString dbusObjectPath)
{
    m_dbusObjectPath = dbusObjectPath;
}

void
PlaceEntry::activate()
{
    QDBusInterface iface("com.canonical.UnityQt", "/dash", "local.DashDeclarativeView");
    iface.call("activatePlaceEntry", m_fileName, m_groupName);
}

void
PlaceEntry::createMenuActions()
{
    // TODO: implement me
}

