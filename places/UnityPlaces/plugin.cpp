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

/* Required otherwise using wnck_set_client_type breaks linking with error:
   undefined reference to `wnck_set_client_type(WnckClientType)'
*/
#include "plugin.h"

#include "blendedimageprovider.h"
#include "cacheeffect.h"
#include "qsortfilterproxymodelqml.h"
#include "windowimageprovider.h"
#include "windowinfo.h"
#include "windowslist.h"

#include <QApplication>
#include <QDeclarativeContext>
#include <QDeclarativeEngine>
#include <QDesktopWidget>
#include <QtDeclarative/qdeclarative.h>

#undef signals
extern "C" {
#include <libwnck/util.h>
}

void UnityPlacesPlugin::registerTypes(const char *uri)
{
    qmlRegisterType<QSortFilterProxyModelQML>(uri, 0, 1, "QSortFilterProxyModelQML");

    qmlRegisterType<WindowInfo>(uri, 0, 1, "WindowInfo");
    qmlRegisterType<WindowsList>(uri, 0, 1, "WindowsList");
    qmlRegisterType<CacheEffect>(uri, 0, 1, "CacheEffect");
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

    /* Critically important to set the client type to pager because wnck
       will pass that type over to the window manager through XEvents.
       Window managers tend to respect orders from pagers to the letter by
       for example bypassing focus stealing prevention.
       Compiz does exactly that in src/event.c:handleEvent(...) in the
       ClientMessage case (line 1702).
       Metacity has a similar policy in src/core/window.c:window_activate(...)
       (line 2951).
    */
    wnck_set_client_type(WNCK_CLIENT_TYPE_PAGER);
}

Q_EXPORT_PLUGIN2(UnityPlaces, UnityPlacesPlugin);
