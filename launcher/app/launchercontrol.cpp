/*
 * Copyright (C) 2011 Canonical, Ltd.
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

#include "launchercontrol.h"
#include "launcheradaptor.h"

#include <QtDBus/QDBusConnection>

static const char* LAUNCHER_DBUS_SERVICE = "com.canonical.Unity2d.Launcher";
static const char* LAUNCHER_DBUS_OBJECT_PATH = "/Launcher";

LauncherControl::LauncherControl(QObject* parent) : QObject(parent)
{
}

LauncherControl::~LauncherControl()
{
    QDBusConnection::sessionBus().unregisterService(LAUNCHER_DBUS_SERVICE);
}

bool
LauncherControl::connectToBus()
{
    bool ok = QDBusConnection::sessionBus().registerService(LAUNCHER_DBUS_SERVICE);
    if (!ok) {
        return false;
    }
    new LauncherAdaptor(this);
    QDBusConnection::sessionBus().registerObject(LAUNCHER_DBUS_OBJECT_PATH, this);

    return true;
}

void
LauncherControl::AddWebFavorite(const QString& url)
{
    Q_EMIT addWebFavorite(url);
}

