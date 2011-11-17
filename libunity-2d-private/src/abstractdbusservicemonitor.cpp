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

#include "abstractdbusservicemonitor.h"

#include <QDBusConnection>
#include <QDBusConnectionInterface>
#include <QDBusReply>

AbstractDBusServiceMonitor::AbstractDBusServiceMonitor(QString service, QString path,
                                                       QString interface, QObject *parent)
    : QObject(parent)
    , m_enabled(false)
    , m_service(service)
    , m_path(path)
    , m_interface(interface)
    , m_watcher(new QDBusServiceWatcher(service, QDBusConnection::sessionBus()))
    , m_dbusInterface(0)
{
}

AbstractDBusServiceMonitor::~AbstractDBusServiceMonitor()
{
    delete m_watcher;
    if (m_dbusInterface) {
        delete m_dbusInterface;
    }
}

bool AbstractDBusServiceMonitor::enabled() const
{
    return m_enabled;
}

/* We don't do this in the constructor because if the service is already up we emit the
   serviceStateChanged() signal during the constructor and we lose it since we can't have any slot
   connected to it already */

void AbstractDBusServiceMonitor::setEnabled(bool enabled)
{
    if (m_enabled != enabled) {
        if (enabled) {
            connect(m_watcher, SIGNAL(serviceRegistered(QString)), SLOT(createInterface(QString)));
            connect(m_watcher, SIGNAL(serviceUnregistered(QString)), SLOT(destroyInterface(QString)));

            // Connect to the service if it's up already
            QDBusConnectionInterface* sessionBus = QDBusConnection::sessionBus().interface();
            QDBusReply<bool> reply = sessionBus->isServiceRegistered(m_service);
            if (reply.isValid() && reply.value()) {
                createInterface(m_service);
            }
        } else {
            if (m_dbusInterface != 0) {
                delete m_dbusInterface;
                m_dbusInterface = 0;
            }
            m_watcher->disconnect(this);
        }

        m_enabled = enabled;
    }
}

void AbstractDBusServiceMonitor::createInterface(QString service)
{
    if (m_dbusInterface != 0) {
        delete m_dbusInterface;
        m_dbusInterface = 0;
    }

    m_dbusInterface = new QDBusInterface(service, m_path, m_interface,
                                         QDBusConnection::sessionBus());
    Q_EMIT serviceStateChanged(true);
}

void AbstractDBusServiceMonitor::destroyInterface(QString service)
{
    Q_UNUSED(service);

    if (m_dbusInterface != 0) {
        delete m_dbusInterface;
        m_dbusInterface = 0;
    }

    Q_EMIT serviceStateChanged(false);
}

QDBusInterface* AbstractDBusServiceMonitor::dbusInterface() const
{
    return m_dbusInterface;
}

#include "abstractdbusservicemonitor.moc"
