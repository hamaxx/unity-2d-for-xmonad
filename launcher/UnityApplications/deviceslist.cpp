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

#include "deviceslist.h"

#include <QDebug>

DevicesList::DevicesList(QObject *parent) :
    QAbstractListModel(parent)
{
    // TODO: load the list of already plugged in devices

    m_volume_monitor = g_volume_monitor_get();
    g_signal_connect(m_volume_monitor, "volume-added",
                     G_CALLBACK(DevicesList::onVolumeAddedProxy), this);
}

DevicesList::~DevicesList()
{
    QList<QDevice*>::iterator iter;
    for(iter = m_devices.begin(); iter != m_devices.end(); ++iter)
    {
        delete *iter;
    }
}

int
DevicesList::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)

    return m_devices.size();
}

QVariant
DevicesList::data(const QModelIndex &index, int role) const
{
    Q_UNUSED(role);

    if (!index.isValid())
        return QVariant();

    return QVariant::fromValue(m_devices.at(index.row()));
}

void
DevicesList::onVolumeAddedProxy(GVolumeMonitor* volume_monitor, GVolume* volume, gpointer data)
{
    DevicesList* _this = static_cast<DevicesList*>(data);
    return _this->onVolumeAdded(volume_monitor, volume);
}

void
DevicesList::onVolumeAdded(GVolumeMonitor* volume_monitor, GVolume* volume)
{
    if (g_volume_can_eject(volume))
    {
        QDevice* device = new QDevice;
        device->setVolume(volume);
        qDebug() << "device added:" << device->name();
        beginInsertRows(QModelIndex(), m_devices.size(), m_devices.size());
        m_devices.append(device);
        endInsertRows();
    }
}

