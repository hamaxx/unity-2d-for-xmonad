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
#include "dashclient.h"

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
static const char* DASH_DBUS_PATH = "/Dash";
static const char* DASH_DBUS_INTERFACE = "com.canonical.Unity2d.Dash";

DashClient::DashClient(QObject* parent)
: QObject(parent)
, m_dashDbusIface(0)
, m_active(false)
, m_alwaysFullScreen(false)
{
    /* Check if the shell is already up and running by asking the bus instead of
       trying to create an instance of the interface. Creating an instance would
       cause D-Bus to activate the shell and we don’t want this to happen, the
       shell should be started on demand only. */
    QDBusConnectionInterface* sessionBusIFace = QDBusConnection::sessionBus().interface();
    QDBusReply<bool> reply = sessionBusIFace->isServiceRegistered(SHELL_DBUS_SERVICE);
    if (reply.isValid() && reply.value()) {
        connectToDash();
    }
    QDBusServiceWatcher* watcher = new QDBusServiceWatcher(SHELL_DBUS_SERVICE,
                                                           QDBusConnection::sessionBus(),
                                                           QDBusServiceWatcher::WatchForRegistration|QDBusServiceWatcher::WatchForUnregistration,
                                                           this);
    connect(watcher, SIGNAL(serviceRegistered(QString)), SLOT(connectToDash()));
    connect(watcher, SIGNAL(serviceUnregistered(QString)), SLOT(onDashDisconnect()));
}

void DashClient::connectToDash()
{
    if (m_dashDbusIface) {
        return;
    }

    m_dashDbusIface = new QDBusInterface(SHELL_DBUS_SERVICE, DASH_DBUS_PATH, DASH_DBUS_INTERFACE,
                                         QDBusConnection::sessionBus(), this);
    connect(m_dashDbusIface, SIGNAL(activeChanged(bool)),
            SLOT(slotActiveChanged(bool)));
    connect(m_dashDbusIface, SIGNAL(alwaysFullScreenChanged(bool)),
            SLOT(slotAlwaysFullScreenChanged(bool)));
    connect(m_dashDbusIface, SIGNAL(screenChanged(int)),
            SLOT(slotScreenChanged(int)));

    QVariant value = m_dashDbusIface->property("active");
    if (value.isValid()) {
        m_active = value.toBool();
    } else {
        UQ_WARNING << "Fetching Dash.active property failed";
    }

    value = m_dashDbusIface->property("alwaysFullScreen");
    if (value.isValid()) {
        m_alwaysFullScreen = value.toBool();
    } else {
        UQ_WARNING << "Fetching Dash.alwaysFullScreen property failed";
    }

    value = m_dashDbusIface->property("screen");
    if (value.isValid()) {
        m_screen = value.toInt();
    } else {
        UQ_WARNING << "Fetching Dash.screen property failed";
    }
}

DashClient* DashClient::instance()
{
    static DashClient* client = new DashClient(qApp);
    return client;
}

void DashClient::onDashDisconnect()
{
    m_active = false;
    delete m_dashDbusIface;
    m_dashDbusIface = NULL;
    Q_EMIT dashDisconnected();
}

void DashClient::slotActiveChanged(bool value)
{
    if (m_active != value) {
        m_active = value;
    }
    Q_EMIT activeChanged(value);
}

bool DashClient::active() const
{
    return m_active;
}

void DashClient::setActive(bool active)
{
    if (!active) {
        // Use m_dashDbusIface only if the dash is running
        if (m_dashDbusIface) {
            m_dashDbusIface->setProperty("active", false);
        }
    } else {
        QDBusInterface iface(SHELL_DBUS_SERVICE, DASH_DBUS_PATH, DASH_DBUS_INTERFACE);
        iface.setProperty("active", true);
    }
}

void DashClient::slotAlwaysFullScreenChanged(bool value)
{
    if (m_alwaysFullScreen != value) {
        m_alwaysFullScreen = value;
    }
    Q_EMIT alwaysFullScreenChanged();
}

bool DashClient::alwaysFullScreen() const
{
    return m_alwaysFullScreen;
}

void DashClient::slotScreenChanged(int screen)
{
    if (screen != m_screen) {
        m_screen = screen;
        Q_EMIT screenChanged(screen);
    }
}

int DashClient::screen() const
{
    return m_screen;
}

bool DashClient::activeInScreen(int screen) const
{
    return active() && m_screen == screen;
}

#include "dashclient.moc"
