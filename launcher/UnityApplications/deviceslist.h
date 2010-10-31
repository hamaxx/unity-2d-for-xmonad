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

#ifndef DEVICESLIST_H
#define DEVICESLIST_H

#include "device.h"

#include <QObject>
#include <QAbstractListModel>

#include <gio/gio.h>

class DevicesList : public QAbstractListModel
{
    Q_OBJECT

public:
    DevicesList(QObject* parent = 0);
    ~DevicesList();

    QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;
    int rowCount(const QModelIndex & parent = QModelIndex()) const;

private:
    GVolumeMonitor* m_volume_monitor;
    QList<Device*> m_devices;

private slots:
    static void onVolumeAddedProxy(GVolumeMonitor* volume_monitor, GVolume* volume, gpointer data);
    void onVolumeAdded(GVolumeMonitor* volume_monitor, GVolume* volume);

    static void onVolumeRemovedProxy(GVolume* volume, gpointer data);
    void onVolumeRemoved(GVolume* volume);
};

#endif // DEVICESLIST_H

