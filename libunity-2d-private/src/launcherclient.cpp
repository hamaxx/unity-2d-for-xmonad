/*
 * This file is part of unity-2d
 *
 * Copyright 2011 Canonical Ltd.
 *
 * Authors:
 * - Aurélien Gâteau <aurelien.gateau@canonical.com>
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
// Self
#include "launcherclient.h"

// Local
#include <debug_p.h>

// Qt
#include <QDBusInterface>
#include <QDBusPendingCall>

static const char* LAUNCHER_DBUS_SERVICE = "com.canonical.Unity2d.Launcher";
static const char* LAUNCHER_DBUS_OBJECT_PATH = "/Launcher";
static const char* LAUNCHER_DBUS_INTERFACE = "com.canonical.Unity2d.Launcher";

const int LauncherClient::MaximumWidth = 66;

struct LauncherClientPrivate
{
    LauncherClient* q;

    QDBusInterface* m_iface;
    bool init()
    {
        if (m_iface) {
            return true;
        }

        m_iface = new QDBusInterface(LAUNCHER_DBUS_SERVICE, LAUNCHER_DBUS_OBJECT_PATH, LAUNCHER_DBUS_INTERFACE,
            QDBusConnection::sessionBus(), q);
        if (!m_iface->isValid()) {
            UQ_WARNING << "Could not connect to Launcher on DBus";
            delete m_iface;
            m_iface = 0;
            return false;
        }
        return true;
    }
};

LauncherClient::LauncherClient(QObject* parent)
: QObject(parent)
, d(new LauncherClientPrivate)
{
    d->q = this;
    d->m_iface = 0;
}

LauncherClient::~LauncherClient()
{
    delete d;
}

void LauncherClient::beginForceVisible()
{
    UQ_RETURN_IF_FAIL(d->init());
    d->m_iface->asyncCall("BeginForceVisible");
}

void LauncherClient::endForceVisible()
{
    UQ_RETURN_IF_FAIL(d->init());
    d->m_iface->asyncCall("EndForceVisible");
}

void LauncherClient::setMouseOverHomeButton(bool value)
{
    UQ_RETURN_IF_FAIL(d->init());
    d->m_iface->asyncCall("SetMouseOverHomeButton", QVariant(value));
}

#include "launcherclient.moc"
