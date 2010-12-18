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
#include "blendedimageprovider.h"

#include <QtDeclarative/qdeclarative.h>
#include <QDeclarativeEngine>
#include <QDeclarativeContext>
#include <QDesktopWidget>
#include <QApplication>

#include "windowimageprovider.h"
#include "windowinfo.h"
#include "windowslist.h"
#include "plugin.h"



void UnityPlacesPlugin::registerTypes(const char *uri)
{
    qmlRegisterType<UnityPlace>(uri, 0, 1, "UnityPlace");
    qmlRegisterType<UnityPlaceEntry>(uri, 0, 1, "UnityPlaceEntry");
    qmlRegisterType<QSortFilterProxyModelQML>(uri, 0, 1, "QSortFilterProxyModelQML");

    qmlRegisterType<WindowInfo>(uri, 0, 1, "WindowInfo");
    qmlRegisterType<WindowsList>(uri, 0, 1, "WindowsList");
}

void UnityPlacesPlugin::initializeEngine(QDeclarativeEngine *engine, const char *uri)
{
    Q_UNUSED(uri);

    engine->addImageProvider(QString("blended"), new BlendedImageProvider);
    engine->addImageProvider(QString("window"), new WindowImageProvider);

    // FIXME: these are probably not very proper here, since clearly not all users
    // of this plugin wish to have this object in their context
    engine->rootContext()->setContextProperty("availableGeometry",
                                              QApplication::desktop()->availableGeometry());
    engine->rootContext()->setContextProperty("screenGeometry",
                                              QApplication::desktop()->geometry());
}

Q_EXPORT_PLUGIN2(UnityPlaces, UnityPlacesPlugin);
