/*
 * This file is part of unity-2d
 *
 * Copyright 2011 Canonical Ltd.
 *
 * Authors:
 * - Aurélien Gâteau <aurelien.gateau@canonical.com>
 *
 * License: GPL v3
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

static const char* SM_DBUS_SERVICE          = "org.gnome.SessionManager";
static const char* SM_DBUS_PATH             = "/org/gnome/SessionManager";
static const char* SM_DBUS_INTERFACE        = "org.gnome.SessionManager";
static const char* SM_CLIENT_DBUS_INTERFACE = "org.gnome.SessionManager.ClientPrivate";

struct GnomeSessionClientPrivate
{
    GnomeSessionClientPrivate(const QString& applicationId)
    : m_applicationId(applicationId)
    {}

    QString m_applicationId;
    QString m_clientPath;
};

GnomeSessionClient::GnomeSessionClient(const QString& applicationId, QObject* parent)
: QObject(parent)
, d(new GnomeSessionClientPrivate(applicationId))
{
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
    QCoreApplication::quit();
}

void GnomeSessionClient::queryEndSession()
{
    QDBusInterface iface(
        SM_DBUS_SERVICE,
        d->m_clientPath,
        SM_CLIENT_DBUS_INTERFACE);
    QDBusReply<void> reply = iface.call("EndSessionResponse", /* is_okay= */ true, /* reason= */ "");
    if (!reply.isValid()) {
        UQ_WARNING << "EndSessionResponse failed:" << reply.error().message();
    }
}

void GnomeSessionClient::endSession()
{
    queryEndSession();
    QCoreApplication::quit();
}

#include "gnomesessionclient.moc"
