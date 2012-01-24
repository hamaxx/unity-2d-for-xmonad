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

#ifndef LAUNCHERDEVICESLIST_H
#define LAUNCHERDEVICESLIST_H

#include "launcherdevice.h"

#include <QObject>
#include <QAbstractListModel>

#include <gio/gio.h>

class LauncherDevicesList : public QAbstractListModel
{
    Q_OBJECT

public:
    LauncherDevicesList(QObject* parent = 0);
    ~LauncherDevicesList();

    QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;
    int rowCount(const QModelIndex & parent = QModelIndex()) const;

private:
    GVolumeMonitor* m_volume_monitor;
    QList<LauncherDevice*> m_devices;
    gulong m_handler_id_volume;
    gulong m_handler_id_mount;

private Q_SLOTS:
    static void onVolumeAddedProxy(GVolumeMonitor* volume_monitor, GVolume* volume, gpointer data);
    void onVolumeAdded(GVolumeMonitor* volume_monitor, GVolume* volume);

    static void onVolumeRemovedProxy(GVolume* volume, gpointer data);
    void onVolumeRemoved(GVolume* volume);

    static void onMountAddedProxy(GVolumeMonitor* volume_monitor, GMount* mount, gpointer data);
    void onMountAdded(GVolumeMonitor* volume_monitor, GMount* mount);

    static void onMountUnmountedProxy(GMount* mount, gpointer data);
    void onMountUnmounted(GMount* mount);
};

#endif // LAUNCHERDEVICESLIST_H

