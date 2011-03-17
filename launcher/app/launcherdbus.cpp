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

#include "launcherdbus.h"
#include "launcheradaptor.h"

// Local
#include <launcherview.h>
#include <visibilitycontroller.h>

// unity-2d
#include <debug_p.h>

// Qt
#include <QtDBus/QDBusConnection>

static const char* LAUNCHER_DBUS_SERVICE = "com.canonical.Unity2d.Launcher";
static const char* LAUNCHER_DBUS_OBJECT_PATH = "/Launcher";

LauncherDBus::LauncherDBus(VisibilityController* visibilityController, LauncherView* view, QObject* parent)
: QObject(parent)
, m_visibilityController(visibilityController)
, m_view(view)
{
}

LauncherDBus::~LauncherDBus()
{
    QDBusConnection::sessionBus().unregisterService(LAUNCHER_DBUS_SERVICE);
}

bool
LauncherDBus::connectToBus()
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
LauncherDBus::AddWebFavorite(const QString& url)
{
    m_view->addWebFavoriteRequested(url);
}

void
LauncherDBus::BeginForceVisible()
{
    UQ_RETURN_IF_FAIL(calledFromDBus());
    m_visibilityController->beginForceVisible(message().service());
}

void
LauncherDBus::EndForceVisible()
{
    UQ_RETURN_IF_FAIL(calledFromDBus());
    m_visibilityController->endForceVisible(message().service());
}
