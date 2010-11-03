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

#include "launchermodel.h"

LauncherModel::LauncherModel(QObject* parent) :
    ListAggregatorModel(parent)
{
    QHash<int, QByteArray> roles;
    roles[0] = "item";
    setRoleNames(roles);

    m_applications = new LauncherApplicationsList;
    aggregateListModel(m_applications);

    m_devices = new LauncherDevicesList;
    aggregateListModel(m_devices);
}

LauncherModel::~LauncherModel()
{
    removeListModel(m_devices);
    delete m_devices;

    removeListModel(m_applications);
    delete m_applications;
}

