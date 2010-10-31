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

LauncherModel::LauncherModel(QObject *parent) :
    QAbstractListModel(parent)
{
    QHash<int, QByteArray> roles;
    roles[0] = "item";
    setRoleNames(roles);

    m_applications = new LauncherApplicationsList;
    QObject::connect(m_applications, SIGNAL(rowsInserted(const QModelIndex&, int, int)),
                     this, SLOT(onApplicationsInserted(const QModelIndex&, int, int)));
    QObject::connect(m_applications, SIGNAL(rowsRemoved(const QModelIndex&, int, int)),
                     this, SLOT(onApplicationsRemoved(const QModelIndex&, int, int)));

    m_devices = new DevicesList;
    QObject::connect(m_devices, SIGNAL(rowsInserted(const QModelIndex&, int, int)),
                     this, SLOT(onDevicesInserted(const QModelIndex&, int, int)));
    QObject::connect(m_devices, SIGNAL(rowsRemoved(const QModelIndex&, int, int)),
                     this, SLOT(onDevicesRemoved(const QModelIndex&, int, int)));
}

LauncherModel::~LauncherModel()
{
    m_list.clear();
    delete m_applications;
    delete m_devices;
}

void
LauncherModel::onApplicationsInserted(const QModelIndex& parent, int first, int last)
{
    beginInsertRows(parent, first, last);
    for (int i = first; i <= last; ++i)
    {
        m_list.insert(i, m_applications->data(createIndex(i, 0)));
    }
    endInsertRows();
}

void
LauncherModel::onApplicationsRemoved(const QModelIndex& parent, int first, int last)
{
    beginRemoveRows(parent, first, last);
    for (int i = first; i <= last; ++i)
    {
        m_list.removeAt(first);
    }
    endRemoveRows();
}

void
LauncherModel::onDevicesInserted(const QModelIndex& parent, int first, int last)
{
    int offset = m_applications->rowCount();
    beginInsertRows(parent, first + offset, last + offset);
    for (int i = first; i <= last; ++i)
    {
        m_list.insert(i + offset, m_devices->data(createIndex(i, 0)));
    }
    endInsertRows();
}

void
LauncherModel::onDevicesRemoved(const QModelIndex& parent, int first, int last)
{
    int offset = m_applications->rowCount();
    beginRemoveRows(parent, first + offset, last + offset);
    for (int i = first; i <= last; ++i)
    {
        m_list.removeAt(first + offset);
    }
    endRemoveRows();
}

int
LauncherModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)

    return m_applications->rowCount() + m_devices->rowCount();
}

QVariant
LauncherModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    int row = index.row();
    if (row < m_applications->rowCount())
    {
        return m_applications->data(index, role);
    }
    else
    {
        QModelIndex new_index = createIndex(row - m_applications->rowCount(), role);
        return m_devices->data(new_index, role);
    }
}

