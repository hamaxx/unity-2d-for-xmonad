/*
 * This file is part of unity-2d
 *
 * Copyright 2011 Canonical Ltd.
 *
 * Authors:
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
// Self
#include "hudclient.h"

// Local
#include "config.h"

// libunity-2d
#include <debug_p.h>

// Qt
#include <QApplication>
#include <QDBusConnection>
#include <QDBusConnectionInterface>
#include <QDBusInterface>
#include <QDBusServiceWatcher>

static const char* SHELL_DBUS_SERVICE = "com.canonical.Unity2d.Shell";
static const char* HUD_DBUS_PATH = "/HUD";
static const char* HUD_DBUS_INTERFACE = "com.canonical.Unity2d.HUD";

HUDClient::HUDClient(QObject* parent)
: QObject(parent)
, m_hudDbusIface(0)
, m_active(false)
{
    /* Check if the shell is already up and running by asking the bus instead of
       trying to create an instance of the interface. Creating an instance would
       cause D-Bus to activate the shell and we don’t want this to happen, the
       shell should be started on demand only. */
    QDBusConnectionInterface* sessionBusIFace = QDBusConnection::sessionBus().interface();
    QDBusReply<bool> reply = sessionBusIFace->isServiceRegistered(SHELL_DBUS_SERVICE);
    if (reply.isValid() && reply.value()) {
        connectToHud();
    }
    QDBusServiceWatcher* watcher = new QDBusServiceWatcher(SHELL_DBUS_SERVICE,
                                                               QDBusConnection::sessionBus(),
                                                               QDBusServiceWatcher::WatchForRegistration|QDBusServiceWatcher::WatchForUnregistration,
                                                               this);
    connect(watcher, SIGNAL(serviceRegistered(QString)), SLOT(connectToHud()));
    connect(watcher, SIGNAL(serviceUnregistered(QString)), SLOT(onHudDisconnect()));
}

void HUDClient::connectToHud()
{
    if (m_hudDbusIface) {
        return;
    }

    m_hudDbusIface = new QDBusInterface(SHELL_DBUS_SERVICE, HUD_DBUS_PATH, HUD_DBUS_INTERFACE,
                                         QDBusConnection::sessionBus(), this);
    connect(m_hudDbusIface, SIGNAL(activeChanged(bool)),
            SLOT(slotActiveChanged(bool)));

    QVariant value = m_hudDbusIface->property("active");
    if (value.isValid()) {
        m_active = value.toBool();
    } else {
        UQ_WARNING << "Fetching HUD.active property failed";
    }
}

HUDClient* HUDClient::instance()
{
    static HUDClient* client = new HUDClient(qApp);
    return client;
}

void HUDClient::onHudDisconnect()
{
    m_active = false;
    delete m_hudDbusIface;
    m_hudDbusIface = NULL;
    Q_EMIT hudDisconnected();
}

void HUDClient::slotActiveChanged(bool value)
{
    if (m_active != value) {
        m_active = value;
    }
    Q_EMIT activeChanged(value);
}

bool HUDClient::active() const
{
    return m_active;
}

void HUDClient::setActive(bool active)
{
    if (!active) {
        // Use m_hudDbusIface only if the shell is running
        if (m_hudDbusIface) {
            m_hudDbusIface->setProperty("active", false);
        }
    } else {
        QDBusInterface iface(SHELL_DBUS_SERVICE, HUD_DBUS_PATH, HUD_DBUS_INTERFACE);
        iface.setProperty("active", true);
    }
}

#include "hudclient.moc"
