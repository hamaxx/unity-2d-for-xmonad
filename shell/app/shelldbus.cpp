/*
 * Copyright (C) 2011 Canonical, Ltd.
 *
 * Authors:
 *  Ugo Riboni <ugo.riboni@canonical.com>
 *  Michał Sawicz <michal.sawicz@canonical.com>
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

#include "shelldbus.h"

// Local
#include <shelldeclarativeview.h>
#include "dashdbus.h"
#include "huddbus.h"

// Qt
#include <QtDBus/QDBusConnection>

static const char* SHELL_DBUS_SERVICE = "com.canonical.Unity2d.Shell";
static const char* DASH_DBUS_OBJECT_PATH = "/Dash";
static const char* HUD_DBUS_OBJECT_PATH = "/HUD";

ShellDBus::ShellDBus(ShellDeclarativeView* view, QObject* parent)
: QObject(parent)
, m_view(view)
{
}

ShellDBus::~ShellDBus()
{
    QDBusConnection::sessionBus().unregisterService(SHELL_DBUS_SERVICE);
}

bool
ShellDBus::connectToBus()
{
    bool ok = QDBusConnection::sessionBus().registerService(SHELL_DBUS_SERVICE);
    if (!ok) {
        return false;
    }

    DashDBus *dashDBus = new DashDBus(m_view, this);
    QDBusConnection::sessionBus().registerObject(DASH_DBUS_OBJECT_PATH, dashDBus);

    HUDDBus *hudDBus = new HUDDBus(m_view, this);
    QDBusConnection::sessionBus().registerObject(HUD_DBUS_OBJECT_PATH, hudDBus);

    return true;
}
