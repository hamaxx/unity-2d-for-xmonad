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

DevicesList::DevicesList(QObject* parent) :
    QAbstractListModel(parent)
{
    m_volume_monitor = g_volume_monitor_get();

    GList* volumes = g_volume_monitor_get_volumes(m_volume_monitor);
    for(GList* li = volumes; li != NULL; li = g_list_next(li))
    {
        GVolume* volume = (GVolume*) li->data;
        onVolumeAdded(m_volume_monitor, volume);
        g_object_unref(volume);
    }
    g_list_free(volumes);

    g_signal_connect(m_volume_monitor, "volume-added",
                     G_CALLBACK(DevicesList::onVolumeAddedProxy), this);
}

DevicesList::~DevicesList()
{
    QList<Device*>::iterator iter;
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
        Device* device = new Device;
        device->setVolume(volume);
        beginInsertRows(QModelIndex(), m_devices.size(), m_devices.size());
        m_devices.append(device);
        endInsertRows();
        g_signal_connect(volume, "removed",
                         G_CALLBACK(DevicesList::onVolumeRemovedProxy), this);
    }
}

void
DevicesList::onVolumeRemovedProxy(GVolume* volume, gpointer data)
{
    DevicesList* _this = static_cast<DevicesList*>(data);
    return _this->onVolumeRemoved(volume);
}

void
DevicesList::onVolumeRemoved(GVolume* volume)
{
    QList<Device*>::iterator iter;
    int i = 0;
    for (iter = m_devices.begin(); iter != m_devices.end(); ++iter)
    {
        if ((*iter)->getVolume() == volume)
        {
            beginRemoveRows(QModelIndex(), i, i);
            Device* device = m_devices.takeAt(i);
            endRemoveRows();
            delete device;
            break;
        }
        ++i;
    }
}

