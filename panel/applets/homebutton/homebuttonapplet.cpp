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

HomeButtonApplet::HomeButtonApplet()
: m_button(new QToolButton)
, m_dashInterface(NULL)
, m_launcherClient(new LauncherClient(this))
{
    m_button->setAutoRaise(true);
    QIcon::setThemeName("unity-icon-theme");
    /* FIXME: Even though the size is set properly the wrong png is loaded.
              unity-icon-theme has 2 visually different distributor-logo icons,
              one 22x22 and another 24x24 (the one we want).
    */
    m_button->setIconSize(QSize(24, 24));
    m_button->setIcon(QIcon::fromTheme("distributor-logo"));
    m_button->setCheckable(true);
    connect(m_button, SIGNAL(clicked()), SLOT(toggleDash()));

    m_button->setStyleSheet(
            "QToolButton { border: none; margin: 0; padding: 0; width: 61 }"
            "QToolButton:checked, QToolButton:pressed {"
            // Use border-image here, not background-image, because bfb_bg_active.png is 56px wide
            "     border-image: url(theme:/bfb_bg_active.png);"
            "}"
    );

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
    Unity2d::Applet::enterEvent(event);
    m_launcherClient->setMouseOverHomeButton(true);
}

void HomeButtonApplet::leaveEvent(QEvent* event)
{
    Unity2d::Applet::leaveEvent(event);
    m_launcherClient->setMouseOverHomeButton(false);
}

#include "homebuttonapplet.moc"
