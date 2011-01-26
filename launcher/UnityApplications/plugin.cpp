/*
 * Copyright (C) 2010-2011 Canonical, Ltd.
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

#include "launcherapplication.h"
#include "place.h"
#include "placeentry.h"
#include "launcherdevice.h"
#include "trash.h"
#include "workspaces.h"
#include "listaggregatormodel.h"
#include "launcherapplicationslist.h"
#include "launcherdeviceslist.h"
#include "launcherplaceslist.h"
#include "iconimageprovider.h"
#include "plugin.h"
#include <QtDeclarative/qdeclarative.h>
#include <QDeclarativeEngine>
#include <QDeclarativeContext>
#include <QAbstractListModel>

void UnityApplicationsPlugin::registerTypes(const char *uri)
{
    qmlRegisterType<ListAggregatorModel>(uri, 0, 1, "ListAggregatorModel");

    qmlRegisterType<LauncherApplicationsList>(uri, 0, 1, "LauncherApplicationsList");
    qmlRegisterType<LauncherApplication>(uri, 0, 1, "LauncherApplication");

    qmlRegisterType<LauncherPlacesList>(uri, 0, 1, "LauncherPlacesList");
    qmlRegisterType<Place>(uri, 0, 1, "Place");
    qmlRegisterType<PlaceEntry>(uri, 0, 1, "PlaceEntry");
    /* DeeListModel is exposed to QML by PlaceEntry */
    qmlRegisterType<DeeListModel>(uri, 0, 1, "DeeListModel");

    qmlRegisterType<LauncherDevicesList>(uri, 0, 1, "LauncherDevicesList");
    qmlRegisterType<LauncherDevice>(uri, 0, 1, "LauncherDevice");

    qmlRegisterType<Trashes>(uri, 0, 1, "Trashes");
    qmlRegisterType<Trash>(uri, 0, 1, "Trash");

    qmlRegisterType<WorkspacesList>(uri, 0, 1, "WorkspacesList");
    qmlRegisterType<Workspaces>(uri, 0, 1, "Workspaces");
}

void UnityApplicationsPlugin::initializeEngine(QDeclarativeEngine *engine, const char *uri)
{
    Q_UNUSED(uri);

    engine->addImageProvider(QString("icons"), new IconImageProvider);
}

Q_EXPORT_PLUGIN2(UnityApplications, UnityApplicationsPlugin);
