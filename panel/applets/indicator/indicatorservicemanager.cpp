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
#include "indicatorservicemanager.h"

// Local
#include "debug_p.h"

// Qt
#include <QAction>
#include <QDBusConnection>
#include <QDBusConnectionInterface>
#include <QDBusMessage>
#include <QDBusServiceWatcher>
#include <QDBusReply>
#include <QDBusInterface>

static const char* INDICATOR_SERVICE_INTERFACE = "org.ayatana.indicator.service";
static const char* INDICATOR_SERVICE_OBJECT    = "/org/ayatana/indicator/service";
static const uint  INDICATOR_SERVICE_VERSION   = 1;

IndicatorServiceManager::IndicatorServiceManager(const char* name, uint version, QObject* parent)
: QObject(parent)
, m_serviceName(name)
, m_serviceVersion(version)
{
    QDBusServiceWatcher* serviceWatcher = new QDBusServiceWatcher(this);
    serviceWatcher->setConnection(QDBusConnection::sessionBus());
    serviceWatcher->addWatchedService(m_serviceName);
    connect(serviceWatcher, SIGNAL(serviceOwnerChanged(QString,QString,QString)),
        SLOT(slotServiceOwnerChanged(QString, QString, QString)));
    connectToService();
}

IndicatorServiceManager::~IndicatorServiceManager()
{
    unwatchService();
}

void IndicatorServiceManager::connectToService()
{
    QDBusConnectionInterface* iface = QDBusConnection::sessionBus().interface();
    QDBusReply<bool> reply = iface->isServiceRegistered(m_serviceName);
    UQ_RETURN_IF_FAIL(reply.isValid());

    if (reply.value()) {
        watchService();
    } else {
        QDBusReply<void> reply = iface->startService(m_serviceName);
        if (!reply.isValid()) {
            UQ_WARNING << reply.error().message();
        }
    }
}

void IndicatorServiceManager::watchService()
{
    QDBusInterface iface(m_serviceName, INDICATOR_SERVICE_OBJECT, INDICATOR_SERVICE_INTERFACE);
    QDBusPendingCall pending = iface.asyncCall("Watch");
    QDBusPendingCallWatcher* watcher = new QDBusPendingCallWatcher(pending, this);
    connect(watcher, SIGNAL(finished(QDBusPendingCallWatcher*)), SLOT(slotWatchFinished(QDBusPendingCallWatcher*)));
}

void IndicatorServiceManager::slotWatchFinished(QDBusPendingCallWatcher* watcher)
{
    QDBusMessage message = watcher->reply();
    delete watcher;
    UQ_RETURN_IF_FAIL(message.type() == QDBusMessage::ReplyMessage);
    QVariantList args = message.arguments();
    UQ_RETURN_IF_FAIL(args.count() == 2);

    uint apiVersion = args.at(0).toUInt();
    uint serviceVersion = args.at(1).toUInt();

    if (apiVersion != INDICATOR_SERVICE_VERSION) {
        UQ_WARNING << "Expected api version=" << INDICATOR_SERVICE_VERSION << "got" << apiVersion << "instead";
        unwatchService();
        return;
    }
    if (serviceVersion != m_serviceVersion) {
        UQ_WARNING << "Expected service version=" << m_serviceVersion << "got" << serviceVersion << "instead";
        unwatchService();
    }
}

void IndicatorServiceManager::unwatchService()
{
    QDBusInterface iface(m_serviceName, INDICATOR_SERVICE_OBJECT, INDICATOR_SERVICE_INTERFACE);
    iface.asyncCall("Unwatch");
}

void IndicatorServiceManager::slotServiceOwnerChanged(const QString& /*name*/, const QString& /*oldOwner*/, const QString& newOwner)
{
    if (newOwner.isNull()) {
        // FIXME: Respawn
    } else {
        watchService();
    }
}

#include "indicatorservicemanager.moc"
