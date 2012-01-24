/*
 * This file is part of unity-2d
 *
 * Copyright 2010 Canonical Ltd.
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
#include "homebuttonapplet.h"

// Local
#include <debug_p.h>

// libunity-2d
#include <launcherclient.h>

// Qt
#include <QHBoxLayout>
#include <QDBusInterface>
#include <QDBusServiceWatcher>
#include <QDBusConnectionInterface>

static const char* DBUS_SERVICE = "com.canonical.Unity2d.Dash";
static const char* DBUS_PATH = "/Dash";
static const char* DBUS_IFACE = "com.canonical.Unity2d.Dash";

HomeButtonApplet::HomeButtonApplet(Unity2dPanel* panel)
: Unity2d::PanelApplet(panel)
, m_button(new HomeButton)
, m_dashInterface(NULL)
, m_launcherClient(new LauncherClient(this))
{
    connect(m_button, SIGNAL(clicked()), SLOT(toggleDash()));

    QHBoxLayout* layout = new QHBoxLayout(this);
    layout->setMargin(0);
    layout->addWidget(m_button);

    /* Check if the dash is already up and running by asking the bus instead of
       trying to create an instance of the interface. Creating an instance
       will cause DBUS to activate the dash and we don't want this to happen, especially
       during startup where the dash is started with a delay. */
    QDBusConnectionInterface* sessionBusIFace = QDBusConnection::sessionBus().interface();
    QDBusReply<bool> reply = sessionBusIFace->isServiceRegistered(DBUS_SERVICE);
    if (reply.isValid() && reply.value() == true) {
        connectToDash();
    } else {
        /* If the dash is not running, setup a notification so that we can
           connect to it later when it comes up */
        QDBusServiceWatcher* serviceWatcher = new QDBusServiceWatcher(DBUS_SERVICE, QDBusConnection::sessionBus(),
                                                                      QDBusServiceWatcher::WatchForRegistration, this);
        connect(serviceWatcher, SIGNAL(serviceRegistered(QString)), SLOT(connectToDash()));
    }
}

void HomeButtonApplet::connectToDash()
{
    if (m_dashInterface != NULL) {
        return;
    }

    m_dashInterface = new QDBusInterface(DBUS_SERVICE, DBUS_PATH, DBUS_IFACE,
                                        QDBusConnection::sessionBus(), this);
    m_button->connect(m_dashInterface, SIGNAL(activeChanged(bool)), SLOT(setChecked(bool)));

    /* Immediately update the home button with the current state of the dash */
    m_button->setChecked(m_dashInterface->property("active").toBool());
}

void HomeButtonApplet::toggleDash()
{
    if (m_dashInterface == NULL || !m_dashInterface->isValid()) {
        connectToDash();
    }

    bool dashActive = m_dashInterface->property("active").toBool();

    if (dashActive) {
        m_dashInterface->setProperty("active", false);
    } else {
        /* Call com.canonical.Unity2d.Dash.activateHome (will set com.canonical.Unity2d.Dash.active to true */
        m_dashInterface->call(QDBus::Block, "activateHome");
    }
}

void HomeButtonApplet::enterEvent(QEvent* event)
{
    QWidget::enterEvent(event);
    m_launcherClient->beginForceVisible();
}

void HomeButtonApplet::leaveEvent(QEvent* event)
{
    QWidget::leaveEvent(event);
    m_launcherClient->endForceVisible();
}

#include "homebuttonapplet.moc"
