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

// Qt
#include <QDBusConnection>
#include <QDBusMessage>
#include <QDBusPendingCall>

static const char* LAUNCHER_DBUS_SERVICE = "com.canonical.Unity2d.Launcher";
static const char* LAUNCHER_DBUS_OBJECT_PATH = "/Launcher";
static const char* LAUNCHER_DBUS_INTERFACE = "com.canonical.Unity2d.Launcher";

const int LauncherClient::MaximumWidth = 66;

struct LauncherClientPrivate
{
    LauncherClient* q;

    void asyncDBusCall(const QString& methodName)
    {
        /* The constructor for QDBusInterface potentially does synchronous
           introspection calls. In contrast, this is really asynchronous.
           See rationale at https://bugs.launchpad.net/unity-2d/+bug/738025. */
        QDBusMessage call = QDBusMessage::createMethodCall(LAUNCHER_DBUS_SERVICE,
                                                           LAUNCHER_DBUS_OBJECT_PATH,
                                                           LAUNCHER_DBUS_INTERFACE,
                                                           methodName);
        QDBusConnection::sessionBus().asyncCall(call);
    }
};

LauncherClient::LauncherClient(QObject* parent)
: QObject(parent)
, d(new LauncherClientPrivate)
{
    d->q = this;
}

LauncherClient::~LauncherClient()
{
    delete d;
}

void LauncherClient::beginForceVisible()
{
    d->asyncDBusCall("BeginForceVisible");
}

void LauncherClient::endForceVisible()
{
    d->asyncDBusCall("EndForceVisible");
}

#include "launcherclient.moc"
