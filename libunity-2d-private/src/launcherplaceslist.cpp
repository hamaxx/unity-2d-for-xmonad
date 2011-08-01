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

#include "launcherplaceslist.h"
#include "place.h"

#include <QDir>

#define PLACES_DIR "/usr/share/unity/places/"
#define FILTER "*.place"

LauncherPlacesList::LauncherPlacesList(QObject* parent) :
    ListAggregatorModel(parent)
{
    QHash<int, QByteArray> roles;
    roles[Place::RoleItem] = "item";
    roles[Place::RoleShowEntry] = "showEntry";
    setRoleNames(roles);

    QDir dir(PLACES_DIR);
    QStringList filters;
    filters << FILTER;
    dir.setNameFilters(filters);
    m_placeFiles = dir.entryList();
    QStringList::const_iterator iter;
    for (iter = m_placeFiles.begin(); iter != m_placeFiles.end(); ++iter) {
        addPlace(dir.absoluteFilePath(*iter));
    }

    // Monitor the directory for new/deleted places
    m_watch = new QFileSystemWatcher;
    m_watch->addPath(dir.absolutePath());
    QObject::connect(m_watch, SIGNAL(directoryChanged(const QString&)),
                     this, SLOT(onDirectoryChanged(const QString&)));
}

LauncherPlacesList::~LauncherPlacesList()
{
    delete m_watch;

    QList<QAbstractItemModel*>::iterator iter;
    for(iter = m_models.begin(); iter != m_models.end(); ) {
        Place* place = static_cast<Place*>(*iter);
        removeListModel(place);
        delete place;
        iter = m_models.begin();
    }
}

Place*
LauncherPlacesList::addPlace(const QString& file)
{
    Place* place = new Place(this);
    aggregateListModel(place);
    place->setFileName(file);
    return place;
}

Place*
LauncherPlacesList::removePlace(const QString& file)
{
    QList<QAbstractItemModel*>::iterator iter;
    for (iter = m_models.begin(); iter != m_models.end(); ++iter) {
        Place* place = static_cast<Place*>(*iter);
        if (place->fileName() == file) {
            removeListModel(place);
            return place;
        }
    }
    return NULL;
}

void
LauncherPlacesList::onDirectoryChanged(const QString& path)
{
    QDir dir(path);
    QStringList filters;
    filters << FILTER;
    dir.setNameFilters(filters);
    QStringList newPlaceFiles = dir.entryList();
    QStringList::const_iterator iter;

    // Any places removed?
    for (iter = m_placeFiles.begin(); iter != m_placeFiles.end(); ++iter) {
        if (!newPlaceFiles.contains(*iter)) {
            Place* place = removePlace(dir.absoluteFilePath(*iter));
            delete place;
        }
    }

    // Any new places?
    for (iter = newPlaceFiles.begin(); iter != newPlaceFiles.end(); ++iter) {
        if (!m_placeFiles.contains(*iter)) {
            addPlace(dir.absoluteFilePath(*iter));
        }
    }

    m_placeFiles = newPlaceFiles;
}

PlaceEntry*
LauncherPlacesList::findPlaceEntry(const QString& fileName, const QString& groupName)
{
    Q_FOREACH(QAbstractItemModel* model, m_models) {
        Place* place = static_cast<Place*>(model);
        if (place->fileName() == fileName) {
            return place->findPlaceEntry(groupName);
        }
    }

    return NULL;
}

void
LauncherPlacesList::startAllPlaceServices()
{
    Q_FOREACH(QAbstractItemModel* model, m_models) {
        Place* place = static_cast<Place*>(model);
        place->connectToRemotePlace();
    }
}

#include "launcherplaceslist.moc"
