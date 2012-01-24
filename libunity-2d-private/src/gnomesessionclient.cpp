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
#include "gnomesessionclient.h"

// Local
#include <debug_p.h>

// Qt
#include <QCoreApplication>
#include <QDBusConnection>
#include <QDBusInterface>
#include <QDBusReply>
#include <QDBusPendingCallWatcher>
#include <QTime>

static const char* SM_DBUS_SERVICE          = "org.gnome.SessionManager";
static const char* SM_DBUS_PATH             = "/org/gnome/SessionManager";
static const char* SM_DBUS_INTERFACE        = "org.gnome.SessionManager";
static const char* SM_CLIENT_DBUS_INTERFACE = "org.gnome.SessionManager.ClientPrivate";

// Number of seconds to wait for gnome-session to call us back
static const int MAX_END_SESSION_WAIT = 3;

struct GnomeSessionClientPrivate
{
    GnomeSessionClientPrivate(const QString& applicationId)
    : m_applicationId(applicationId)
    {}

    QString m_applicationId;
    QString m_clientPath;
    bool m_waitingForEndSession;

    bool sendEndSessionResponse()
    {
        UQ_DEBUG;
        QDBusInterface iface(
            SM_DBUS_SERVICE,
            m_clientPath,
            SM_CLIENT_DBUS_INTERFACE);
        QDBusReply<void> reply = iface.call("EndSessionResponse", /* is_okay= */ true, /* reason= */ "");
        if (reply.isValid()) {
            return true;
        } else {
            UQ_WARNING << "EndSessionResponse failed:" << reply.error().message();
            return false;
        }
    }
};

GnomeSessionClient::GnomeSessionClient(const QString& applicationId, QObject* parent)
: QObject(parent)
, d(new GnomeSessionClientPrivate(applicationId))
{
    d->m_waitingForEndSession = false;
    connect(QCoreApplication::instance(), SIGNAL(aboutToQuit()),
        SLOT(waitForEndSession()));
}

GnomeSessionClient::~GnomeSessionClient()
{
    delete d;
}

void GnomeSessionClient::connectToSessionManager()
{
    QString startupId = QString::fromLocal8Bit(qgetenv("DESKTOP_AUTOSTART_ID"));

    QDBusInterface* managerIface = new QDBusInterface(
        SM_DBUS_SERVICE,
        SM_DBUS_PATH,
        SM_DBUS_INTERFACE,
        QDBusConnection::sessionBus(),
        this);

    QDBusPendingCall call = managerIface->asyncCall("RegisterClient", d->m_applicationId, startupId);
    QDBusPendingCallWatcher* watcher = new QDBusPendingCallWatcher(call, this);
    connect(watcher, SIGNAL(finished(QDBusPendingCallWatcher*)),
        SLOT(slotRegisterClientFinished(QDBusPendingCallWatcher*)));
}

void GnomeSessionClient::slotRegisterClientFinished(QDBusPendingCallWatcher* watcher)
{
    QDBusPendingReply<QDBusObjectPath> reply = *watcher;
    watcher->deleteLater();
    if (!reply.isValid()) {
        UQ_WARNING << "Failed to register with GnomeSession:" << reply.error().message();
        return;
    }

    d->m_waitingForEndSession = true;
    QDBusConnection bus = QDBusConnection::sessionBus();
    d->m_clientPath = reply.value().path();

    bus.connect(SM_DBUS_SERVICE, d->m_clientPath, SM_CLIENT_DBUS_INTERFACE,
        "Stop", "", this, SLOT(stop()));
    bus.connect(SM_DBUS_SERVICE, d->m_clientPath, SM_CLIENT_DBUS_INTERFACE,
        "QueryEndSession", "u", this, SLOT(queryEndSession()));
    bus.connect(SM_DBUS_SERVICE, d->m_clientPath, SM_CLIENT_DBUS_INTERFACE,
        "EndSession", "u", this, SLOT(endSession()));
}

void GnomeSessionClient::stop()
{
    UQ_DEBUG;
    QCoreApplication::quit();
}

void GnomeSessionClient::queryEndSession()
{
    UQ_DEBUG;
    if (!d->sendEndSessionResponse()) {
      d->m_waitingForEndSession = false;
    }
}

void GnomeSessionClient::endSession()
{
    UQ_DEBUG;
    d->sendEndSessionResponse();
    d->m_waitingForEndSession = false;
    QCoreApplication::quit();
}

void GnomeSessionClient::waitForEndSession()
{
    if (!d->m_waitingForEndSession) {
        return;
    }

    UQ_DEBUG << "Application is about to quit, waiting for gnome-session to call us back";
    QTime watchDog;
    watchDog.start();
    while (d->m_waitingForEndSession && watchDog.elapsed() < MAX_END_SESSION_WAIT * 1000) {
        QCoreApplication::processEvents(QEventLoop::ExcludeUserInputEvents);
    }
    if (d->m_waitingForEndSession) {
        UQ_WARNING << "gnome-session did not call back after" << MAX_END_SESSION_WAIT << "seconds, leaving";
    }
}

#include "gnomesessionclient.moc"
