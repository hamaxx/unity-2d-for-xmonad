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
#include <QDesktopWidget>
#include <QRect>

static const char* DASH_DBUS_SERVICE = "com.canonical.Unity2d.Dash";
static const char* DASH_DBUS_PATH = "/Dash";
static const char* DASH_DBUS_INTERFACE = "com.canonical.Unity2d.Dash";

static const int DASH_MIN_SCREEN_WIDTH = 1280;
static const int DASH_MIN_SCREEN_HEIGHT = 1084;

DashClient::DashClient(QObject* parent)
: QObject(parent)
, m_dashDbusIface(0)
, m_dashActive(false)
, m_hudActive(false)
, m_alwaysFullScreen(false)
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

    connect(QApplication::desktop(), SIGNAL(resized(int)), SLOT(updateAlwaysFullScreen()));

    // FIXME: we need to use a queued connection here otherwise QConf will deadlock for some reason
    // when we read any property from the slot (which we need to do). We need to check why this
    // happens and report a bug to dconf-qt to get it fixed.
    connect(&unity2dConfiguration(), SIGNAL(formFactorChanged(QString)),
                                     SLOT(updateAlwaysFullScreen()), Qt::QueuedConnection);
    updateAlwaysFullScreen();
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
    connect(m_dashDbusIface, SIGNAL(hudActiveChanged(bool)),
            SLOT(slotHudActiveChanged(bool)));

    QVariant value = m_dashDbusIface->property("active");
    if (value.isValid()) {
        m_dashActive = value.toBool();
    } else {
        UQ_WARNING << "Fetching Dash.active property failed";
    }
    value = m_dashDbusIface->property("hudActive");
    if (value.isValid()) {
        m_hudActive = value.toBool();
    } else {
        UQ_WARNING << "Fetching Dash.hudActive property failed";
    }
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
    }
    Q_EMIT activeChanged(value);
}

void DashClient::slotHudActiveChanged(bool value)
{
    if (m_hudActive != value) {
        m_hudActive = value;
    }
    Q_EMIT hudActiveChanged(value);
}

bool DashClient::active() const
{
    return m_dashActive;
}

bool DashClient::hudActive() const
{
    return m_hudActive;
}

void DashClient::setActive(bool active)
{
    if (!active) {
        // Use m_dashDbusIface only if the dash is running
        if (m_dashDbusIface) {
            m_dashDbusIface->setProperty("active", false);
        }
    } else {
        QDBusInterface iface(DASH_DBUS_SERVICE, DASH_DBUS_PATH, DASH_DBUS_INTERFACE);
        iface.setProperty("active", true);
    }
}

void DashClient::setHudActive(bool active)
{
    if (!active) {
        // Use m_dashDbusIface only if the shell is running
        if (m_dashDbusIface) {
            m_dashDbusIface->setProperty("hudActive", false);
        }
    } else {
        QDBusInterface iface(DASH_DBUS_SERVICE, DASH_DBUS_PATH, DASH_DBUS_INTERFACE);
        iface.setProperty("hudActive", true);
    }
}

QSize DashClient::minimumSizeForDesktop()
{
    return QSize(DASH_MIN_SCREEN_WIDTH, DASH_MIN_SCREEN_HEIGHT);
}

void DashClient::updateAlwaysFullScreen()
{
    bool alwaysFullScreen;
    if (unity2dConfiguration().property("formFactor").toString() != "desktop") {
        alwaysFullScreen = true;
    } else {
        QRect rect = QApplication::desktop()->screenGeometry(QPoint());
        QSize minSize = minimumSizeForDesktop();
        alwaysFullScreen = rect.width() < minSize.width() && rect.height() < minSize.height();
    }

    if (m_alwaysFullScreen != alwaysFullScreen) {
        m_alwaysFullScreen = alwaysFullScreen;
        Q_EMIT alwaysFullScreenChanged();
    }
}

bool DashClient::alwaysFullScreen() const
{
    return m_alwaysFullScreen;
}

#include "dashclient.moc"
