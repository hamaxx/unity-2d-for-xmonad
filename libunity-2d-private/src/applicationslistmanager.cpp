/*
 * Copyright (C) 2012 Canonical, Ltd.
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

#include "applicationslistmanager.h"

// unity-2d
#include "applicationslist.h"
#include "applicationslistdbus.h"
#include "debug_p.h"

// Qt
#include <QDBusConnection>
#include <QDBusMessage>

#define DBUS_SERVICE_UNITY "com.canonical.Unity"
#define DBUS_SERVICE_LAUNCHER_ENTRY "com.canonical.Unity.LauncherEntry"
#define DBUS_SERVICE_LAUNCHER "com.canonical.Unity.Launcher"
#define DBUS_OBJECT_LAUNCHER "/com/canonical/Unity/Launcher"

ApplicationsListManager::ApplicationsListManager()
{
    QDBusConnection session = QDBusConnection::sessionBus();
    /* FIXME: libunity will send out the Update signal for LauncherEntries
       only if it finds com.canonical.Unity on the bus, so let's just quickly
       register ourselves as Unity here. Should be moved somewhere else more proper */
    if (!session.registerService(DBUS_SERVICE_UNITY)) {
        UQ_WARNING << "The name" << DBUS_SERVICE_UNITY << "is already taken on DBUS";
    } else {
        /* Set ourselves up to receive any Update signal coming from any
           LauncherEntry */
        session.connect(QString(), QString(),
                        DBUS_SERVICE_LAUNCHER_ENTRY, "Update",
                        this, SLOT(onRemoteEntryUpdated(QString,QMap<QString,QVariant>)));
    }

    if (!session.registerService(DBUS_SERVICE_LAUNCHER)) {
        UQ_WARNING << "The name" << DBUS_SERVICE_LAUNCHER << "is already taken on DBUS";
    } else {
        /* Set ourselves up to receive a method call from Software Center asking us to add
           to favorites an application that is being installed and that the user requested
           to be added. */
        ApplicationsListDBUS *dbusAdapter = new ApplicationsListDBUS(this);
        if (!session.registerObject(DBUS_OBJECT_LAUNCHER, dbusAdapter,
                                    QDBusConnection::ExportAllSlots)) {
            UQ_WARNING << "The object" << DBUS_OBJECT_LAUNCHER << "on" << DBUS_SERVICE_LAUNCHER
                       << "is already present on DBUS.";
        }
    }
}

ApplicationsListManager *ApplicationsListManager::instance()
{
    static ApplicationsListManager manager;
    return &manager;
}

void ApplicationsListManager::addList(ApplicationsList *list)
{
    m_lists += list;
}

void ApplicationsListManager::removeList(ApplicationsList *list)
{
    m_lists -= list;
}

void ApplicationsListManager::insertFavoriteApplication(const QString& desktop_file)
{
    Q_FOREACH(ApplicationsList *list, m_lists) {
        list->insertFavoriteApplication(desktop_file);
    }
}

void ApplicationsListManager::insertWebFavorite(const QUrl& url)
{
    Q_FOREACH(ApplicationsList *list, m_lists) {
        list->insertWebFavorite(url);
    }
}

void
ApplicationsListManager::onRemoteEntryUpdated(QString applicationURI, QMap<QString, QVariant> properties)
{
    UQ_RETURN_IF_FAIL(calledFromDBus());
    QString sender = message().service();
    QString desktopFile;
    if (applicationURI.indexOf("application://") == 0) {
        desktopFile = applicationURI.mid(14);
    } else {
        UQ_WARNING << "Ignoring update that didn't come from an application:// URI but from:" << applicationURI;
        return;
    }

    Q_FOREACH(ApplicationsList *list, m_lists) {
        list->remoteEntryUpdated(desktopFile, sender, applicationURI, properties);
    }
}

#include "applicationslistmanager.moc"