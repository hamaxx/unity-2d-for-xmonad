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

#include "launcherdeviceslist.h"
#include "launcherapplicationslist.h"
#include "launcherplaceslist.h"

#include "listaggregatormodel.h"

class LauncherModel : public ListAggregatorModel
{
    Q_OBJECT

public:
    LauncherModel(QObject* parent = 0);
    ~LauncherModel();

    Q_PROPERTY(LauncherApplicationsList* applications READ applications)

    LauncherApplicationsList* applications() { return m_applications; }

private:
    LauncherApplicationsList* m_applications;
    LauncherPlacesList* m_places;
    LauncherDevicesList* m_devices;
};

#endif // LAUNCHERMODEL_H

