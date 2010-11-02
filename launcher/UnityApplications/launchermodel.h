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

#ifndef LAUNCHERMODEL_H
#define LAUNCHERMODEL_H

#include "launcherapplicationslist.h"
#include "launcherdeviceslist.h"

#include <QAbstractListModel>
#include <QObject>
#include <QVariant>

class LauncherModel : public QAbstractListModel
{
    Q_OBJECT

public:
    LauncherModel(QObject *parent = 0);
    ~LauncherModel();

    QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;
    int rowCount(const QModelIndex & parent = QModelIndex()) const;

private slots:
    void onApplicationsInserted(const QModelIndex& parent, int first, int last);
    void onApplicationsRemoved(const QModelIndex& parent, int first, int last);
    void onDevicesInserted(const QModelIndex& parent, int first, int last);
    void onDevicesRemoved(const QModelIndex& parent, int first, int last);

private:
    LauncherApplicationsList* m_applications;
    LauncherDevicesList* m_devices;

    QVariantList m_list;
};

#endif // LAUNCHERMODEL_H

