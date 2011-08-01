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
    for(GList* li = volumes; li != NULL; li = g_list_next(li)) {
        GVolume* volume = (GVolume*) li->data;
        onVolumeAdded(m_volume_monitor, volume);
        g_object_unref(volume);
    }
    g_list_free(volumes);

    GList* mounts = g_volume_monitor_get_mounts(m_volume_monitor);
    for(GList* li = mounts; li != NULL; li = g_list_next(li)) {
        GMount* mount = (GMount*) li->data;
        onMountAdded(m_volume_monitor, mount);
        g_object_unref(mount);
    }
    g_list_free(mounts);

    m_handler_id_volume = g_signal_connect(m_volume_monitor, "volume-added",
        G_CALLBACK(LauncherDevicesList::onVolumeAddedProxy), this);
    m_handler_id_mount = g_signal_connect(m_volume_monitor, "mount-added",
        G_CALLBACK(LauncherDevicesList::onMountAddedProxy), this);
}

LauncherDevicesList::~LauncherDevicesList()
{
    g_signal_handler_disconnect(m_volume_monitor, m_handler_id_volume);
    g_signal_handler_disconnect(m_volume_monitor, m_handler_id_mount);
    g_object_unref(m_volume_monitor);

    QList<LauncherDevice*>::iterator iter;
    for (iter = m_devices.begin(); iter != m_devices.end(); ++iter) {
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

    if (!index.isValid()) {
        return QVariant();
    }

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
    if (g_volume_can_eject(volume)) {
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
    for (iter = m_devices.begin(); iter != m_devices.end(); ++iter) {
        if ((*iter)->getVolume() == volume) {
            beginRemoveRows(QModelIndex(), i, i);
            LauncherDevice* device = m_devices.takeAt(i);
            endRemoveRows();
            delete device;
            break;
        }
        ++i;
    }
}

void
LauncherDevicesList::onMountAddedProxy(GVolumeMonitor* volume_monitor, GMount* mount, gpointer data)
{
    LauncherDevicesList* _this = static_cast<LauncherDevicesList*>(data);
    return _this->onMountAdded(volume_monitor, mount);
}

void
LauncherDevicesList::onMountAdded(GVolumeMonitor* volume_monitor, GMount* mount)
{
    if (!g_mount_can_unmount(mount)) {
        return;
    }

    GVolume* volume = g_mount_get_volume(mount);
    if (volume == NULL) {
        return;
    }

    QList<LauncherDevice*>::const_iterator iter;
    for (iter = m_devices.begin(); iter != m_devices.end(); ++iter) {
        if ((*iter)->getVolume() == volume) {
            /* The device is already displayed. */
            g_object_unref(volume);
            return;
        }
    }

    LauncherDevice* device = new LauncherDevice;
    device->setVolume(volume);
    beginInsertRows(QModelIndex(), m_devices.size(), m_devices.size());
    m_devices.append(device);
    endInsertRows();
    g_signal_connect(mount, "unmounted",
                     G_CALLBACK(LauncherDevicesList::onMountUnmountedProxy), this);

    g_object_unref(volume);
}

void
LauncherDevicesList::onMountUnmountedProxy(GMount* mount, gpointer data)
{
    LauncherDevicesList* _this = static_cast<LauncherDevicesList*>(data);
    return _this->onMountUnmounted(mount);
}

void
LauncherDevicesList::onMountUnmounted(GMount* mount)
{
    QList<LauncherDevice*>::iterator iter;
    int i = 0;
    for (iter = m_devices.begin(); iter != m_devices.end(); ++iter) {
        /* At this point the mount is unmounted, so we can't rely on comparing
           it to the current device's mount. */
        GVolume* volume = (*iter)->getVolume();
        if (!g_volume_can_eject(volume)) {
            GMount* m = g_volume_get_mount(volume);
            if (m == NULL) {
                /* The volume can't eject and it is not mounted: this is our
                   device. */
                beginRemoveRows(QModelIndex(), i, i);
                LauncherDevice* device = m_devices.takeAt(i);
                endRemoveRows();
                delete device;
                break;
            } else {
                g_object_unref(m);
            }
        }
        ++i;
    }
}

#include "launcherdeviceslist.moc"
