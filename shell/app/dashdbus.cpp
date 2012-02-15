/*
 * Copyright (C) 2011 Canonical, Ltd.
 *
 * Authors:
 *  Ugo Riboni <ugo.riboni@canonical.com>
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

#include "dashdbus.h"
#include "dashadaptor.h"

// Local
#include <shellmanager.h>

// Qt
#include <QtDBus/QDBusConnection>

static const char* DASH_DBUS_SERVICE = "com.canonical.Unity2d.Dash";
static const char* DASH_DBUS_OBJECT_PATH = "/Dash";

DashDBus::DashDBus(ShellManager* manager, QObject* parent)
: QObject(parent)
, m_manager(manager)
{
    connect(m_manager, SIGNAL(dashActiveChanged(bool)), SIGNAL(activeChanged(bool)));
    connect(m_manager, SIGNAL(dashActiveLensChanged(QString)), SIGNAL(activeLensChanged(QString)));
}

DashDBus::~DashDBus()
{
    QDBusConnection::sessionBus().unregisterService(DASH_DBUS_SERVICE);
}

bool
DashDBus::connectToBus()
{
    bool ok = QDBusConnection::sessionBus().registerService(DASH_DBUS_SERVICE);
    if (!ok) {
        return false;
    }
    new DashAdaptor(this);
    QDBusConnection::sessionBus().registerObject(DASH_DBUS_OBJECT_PATH, this);

    return true;
}

void
DashDBus::activateHome()
{
    Q_EMIT m_manager->dashActivateHome();
}

void
DashDBus::activateLens(const QString& lensId)
{
    Q_EMIT m_manager->dashActivateLens(lensId);
}

bool
DashDBus::active() const
{
    return m_manager->dashActive();
}

void
DashDBus::setActive(bool active)
{
    m_manager->setDashActive(active);
}

QString
DashDBus::activeLens() const
{
    return m_manager->dashActiveLens();
}

void
DashDBus::setActiveLens(QString activeLens)
{
    m_manager->setDashActiveLens(activeLens);
}
