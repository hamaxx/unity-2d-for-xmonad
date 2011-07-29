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

#ifndef LAUNCHERPLACESLIST_H
#define LAUNCHERPLACESLIST_H

#include "listaggregatormodel.h"

#include <QStringList>
#include <QFileSystemWatcher>

class Place;
class PlaceEntry;

class LauncherPlacesList : public ListAggregatorModel
{
    Q_OBJECT

public:
    LauncherPlacesList(QObject* parent = 0);
    ~LauncherPlacesList();

    Q_INVOKABLE PlaceEntry* findPlaceEntry(const QString& fileName, const QString& groupName);
    Q_INVOKABLE void startAllPlaceServices();

private:
    QStringList m_placeFiles;
    QFileSystemWatcher* m_watch;

    Place* addPlace(const QString& file);
    Place* removePlace(const QString& file);

private Q_SLOTS:
    void onDirectoryChanged(const QString&);
};

#endif // LAUNCHERPLACESLIST_H

