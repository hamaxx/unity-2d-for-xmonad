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

#include "launcherdeviceslist.h"

LauncherDevicesList::LauncherDevicesList(QObject* parent) :
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

    m_handler_id = g_signal_connect(m_volume_monitor, "volume-added",
        G_CALLBACK(LauncherDevicesList::onVolumeAddedProxy), this);
}

LauncherDevicesList::~LauncherDevicesList()
{
    g_signal_handler_disconnect(m_volume_monitor, m_handler_id);
    g_object_unref(m_volume_monitor);

    QList<LauncherDevice*>::iterator iter;
    for(iter = m_devices.begin(); iter != m_devices.end(); ++iter)
    {
        delete *iter;
    }
}

int
LauncherDevicesList::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)

    return m_devices.size();
}

QVariant
LauncherDevicesList::data(const QModelIndex &index, int role) const
{
    Q_UNUSED(role);

    if (!index.isValid())
        return QVariant();

    return QVariant::fromValue(m_devices.at(index.row()));
}

void
LauncherDevicesList::onVolumeAddedProxy(GVolumeMonitor* volume_monitor, GVolume* volume, gpointer data)
{
    LauncherDevicesList* _this = static_cast<LauncherDevicesList*>(data);
    return _this->onVolumeAdded(volume_monitor, volume);
}

void
LauncherDevicesList::onVolumeAdded(GVolumeMonitor* volume_monitor, GVolume* volume)
{
    if (g_volume_can_eject(volume))
    {
        LauncherDevice* device = new LauncherDevice;
        device->setVolume(volume);
        beginInsertRows(QModelIndex(), m_devices.size(), m_devices.size());
        m_devices.append(device);
        endInsertRows();
        g_signal_connect(volume, "removed",
                         G_CALLBACK(LauncherDevicesList::onVolumeRemovedProxy), this);
    }
}

void
LauncherDevicesList::onVolumeRemovedProxy(GVolume* volume, gpointer data)
{
    LauncherDevicesList* _this = static_cast<LauncherDevicesList*>(data);
    return _this->onVolumeRemoved(volume);
}

void
LauncherDevicesList::onVolumeRemoved(GVolume* volume)
{
    QList<LauncherDevice*>::iterator iter;
    int i = 0;
    for (iter = m_devices.begin(); iter != m_devices.end(); ++iter)
    {
        if ((*iter)->getVolume() == volume)
        {
            beginRemoveRows(QModelIndex(), i, i);
            LauncherDevice* device = m_devices.takeAt(i);
            endRemoveRows();
            delete device;
            break;
        }
        ++i;
    }
}

