/*
 * Copyright (C) 2010 Canonical, Ltd.
 *
 * Authors:
 *  Florian Boucault <florian.boucault@canonical.com>
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

#include "unity_place.h"
#include "unity_place_entry.h"
#include "qsortfilterproxymodelqml.h"
#include "plugin.h"
#include <QtDeclarative/qdeclarative.h>
#include <QDeclarativeEngine>


void UnityPlacesPlugin::registerTypes(const char *uri)
{
    qmlRegisterType<UnityPlace>(uri, 0, 1, "UnityPlace");
    qmlRegisterType<UnityPlaceEntry>(uri, 0, 1, "UnityPlaceEntry");
    qmlRegisterType<QSortFilterProxyModelQML>(uri, 0, 1, "QSortFilterProxyModelQML");
}

void UnityPlacesPlugin::initializeEngine(QDeclarativeEngine *engine, const char *uri)
{
    Q_UNUSED(uri);
}

Q_EXPORT_PLUGIN2(UnityPlaces, UnityPlacesPlugin);
