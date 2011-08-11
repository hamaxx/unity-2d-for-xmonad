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

// libunity-2d
#include <debug_p.h>

// Qt
#include <QApplication>
#include <QDBusConnection>
#include <QDBusConnectionInterface>
#include <QDBusInterface>
#include <QDBusServiceWatcher>

static const char* DASH_DBUS_SERVICE = "com.canonical.Unity2d.Dash";
static const char* DASH_DBUS_PATH = "/Dash";
static const char* DASH_DBUS_INTERFACE = "com.canonical.Unity2d.Dash";

DashClient::DashClient(QObject* parent)
: QObject(parent)
, m_dashDbusIface(0)
, m_dashActive(false)
{
    /* Check if the dash is already up and running by asking the bus instead of
       trying to create an instance of the interface. Creating an instance would
       cause D-Bus to activate the dash and we don’t want this to happen, the
       dash should be started on demand only. */
    QDBusConnectionInterface* sessionBusIFace = QDBusConnection::sessionBus().interface();
    QDBusReply<bool> reply = sessionBusIFace->isServiceRegistered(DASH_DBUS_SERVICE);
    if (reply.isValid() && reply.value()) {
        connectToDash();
    } else {
        /* The dash is not running: monitor its registration on the bus so we
           can connect to it when it comes up. */
        QDBusServiceWatcher* watcher = new QDBusServiceWatcher(DASH_DBUS_SERVICE,
                                                               QDBusConnection::sessionBus(),
                                                               QDBusServiceWatcher::WatchForRegistration,
                                                               this);
        connect(watcher, SIGNAL(serviceRegistered(QString)), SLOT(connectToDash()));
    }
}

void DashClient::connectToDash()
{
    if (m_dashDbusIface) {
        return;
    }

    m_dashDbusIface = new QDBusInterface(DASH_DBUS_SERVICE, DASH_DBUS_PATH, DASH_DBUS_INTERFACE,
                                         QDBusConnection::sessionBus(), this);
    connect(m_dashDbusIface, SIGNAL(activeChanged(bool)),
            SLOT(slotDashActiveChanged(bool)));
    connect(m_dashDbusIface, SIGNAL(activeLensChanged(const QString&)),
            SLOT(slotDashActiveLensChanged(const QString&)));

    QVariant value = m_dashDbusIface->property("active");
    if (value.isValid()) {
        m_dashActive = value.toBool();
    } else {
        UQ_WARNING << "Fetching Dash.active property failed";
    }
    value = m_dashDbusIface->property("activeLens");
    if (value.isValid()) {
        m_dashActiveLens = value.toString();
    } else {
        UQ_WARNING << "Fetching Dash.activeLens property failed";
    }

    updateActivePage();
}

DashClient* DashClient::instance()
{
    static DashClient* client = new DashClient(qApp);
    return client;
}

void DashClient::slotDashActiveChanged(bool value)
{
    if (m_dashActive != value) {
        m_dashActive = value;
        updateActivePage();
    }
}

void DashClient::slotDashActiveLensChanged(const QString& lens)
{
    if (m_dashActiveLens != lens) {
        m_dashActiveLens = lens;
        updateActivePage();
    }
}

QString DashClient::activePage() const
{
    return m_activePage;
}

void DashClient::setActivePage(const QString& page, const QString& lensId)
{
    if (m_activePage == page) {
        return;
    }
    if (page.isEmpty()) {
        // Use m_dashDbusIface to close the dash, but only if it is running
        if (m_dashDbusIface) {
            m_dashDbusIface->setProperty("active", false);
        }
        return;
    }
    // Use a separate QDBusInterface so that the dash is started if it is not
    // already running
    QDBusInterface iface(DASH_DBUS_SERVICE, DASH_DBUS_PATH, DASH_DBUS_INTERFACE);
    if (page == "home") {
        iface.asyncCall("activateHome");
    } else {
        iface.asyncCall("activateLens", lensId);
    }
}

void DashClient::updateActivePage()
{
    QString activePage;
    if (m_dashActive) {
        activePage = m_dashActiveLens.isEmpty() ? "home" : m_dashActiveLens;
    }

    if (m_activePage != activePage) {
        m_activePage = activePage;
        activePageChanged(m_activePage);
    }
}

#include "dashclient.moc"
