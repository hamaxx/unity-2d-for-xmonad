/*
 * Copyright (C) 2010 Canonical, Ltd.
 *
 * Authors:
 *  Olivier Tilloy <olivier.tilloy@canonical.com>
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

#include "launcherapplication.h"
#include "place.h"

// libunity-2d
#include <unity2dtr.h>
#include <debug_p.h>

#include <QHash>
#include <QByteArray>
#include <QStringList>
#include <QDBusPendingReply>
#include <QDBusServiceWatcher>
#include <QDBusConnectionInterface>
#include <QTimer>
#include <QUrl>
#include <QDesktopServices>
#include <QDBusReply>

static const char* UNITY_PLACE_INTERFACE = "com.canonical.Unity.Place";
static const char* UNITY_ACTIVATION_INTERFACE = "com.canonical.Unity.Activation";

Place::Place(QObject* parent) :
    QAbstractListModel(parent),
    m_file(NULL),
    m_online(false),
    m_dbusIface(NULL),
    m_querying(false)
{
    QHash<int, QByteArray> roles;
    roles[RoleItem] = "item";
    roles[RoleShowEntry] = "showEntry";
    setRoleNames(roles);

    m_serviceWatcher = new QDBusServiceWatcher(this);
    m_serviceWatcher->setConnection(QDBusConnection::sessionBus());
    connect(m_serviceWatcher, SIGNAL(serviceRegistered(QString)),
            SLOT(onPlaceServiceRegistered()));
    connect(m_serviceWatcher, SIGNAL(serviceUnregistered(QString)),
            SLOT(onPlaceServiceUnregistered()));
}

Place::Place(const Place &other)
{
    if (other.m_file != NULL) {
        setFileName(other.m_file->fileName());
    }
}

Place::~Place()
{
    delete m_dbusIface;
    delete m_file;
    m_entries.clear();
    m_static_entries.clear();
}

QString
Place::fileName() const
{
    return m_file->fileName();
}

void
Place::setFileName(const QString &file)
{
    if (!m_dbusName.isNull()) {
        m_serviceWatcher->removeWatchedService(m_dbusName);
    }

    if (m_dbusIface != NULL) {
        delete m_dbusIface;
    }

    m_file = new QSettings(file, QSettings::IniFormat);
    if (m_file->childGroups().contains("Place")) {
        m_dbusName = m_file->value("Place/DBusName").toString();
        m_dbusObjectPath = m_file->value("Place/dbusObjectPath").toString();

        QString gettextDomain = m_file->value("Desktop Entry/X-Ubuntu-Gettext-Domain").toString();

        QStringList entries = m_file->childGroups().filter("Entry:");
        QStringList::const_iterator iter;
        uint i = 0;
        for(iter = entries.begin(); iter != entries.end(); ++iter) {
            PlaceEntry* entry = new PlaceEntry(this);
            entry->setFileName(file);
            entry->setGroupName(iter->mid(6));
            m_file->beginGroup(*iter);
            entry->setDbusName(m_dbusName);
            entry->setDbusObjectPath(m_file->value("DBusObjectPath").toString());
            entry->setName(u2dTr(m_file->value("Name").toString().toUtf8().constData(),
                                 gettextDomain.toUtf8().constData()));
            entry->setIcon(m_file->value("Icon").toString());
            entry->setSearchHint(u2dTr(m_file->value("SearchHint").toString().toUtf8().constData(),
                                 gettextDomain.toUtf8().constData()));
            if (m_file->contains("Shortcut")) {
                QString value = m_file->value("Shortcut").toString();
                if (value.size() == 1) {
                    Qt::Key key = (Qt::Key) value.at(0).toUpper().unicode();
                    entry->setShortcutKey(key);
                } else {
                    /* Note: some text editors insert the decomposed form of
                       e.g. accented characters (e.g. 0xc3 + 0xa9 for "É"
                       instead of the canonical form 0xc9). Unfortunately Qt
                       doesn’t seem to be able to perform composition, so in
                       such cases setting the shortcut key fails. See
                       http://www.unicode.org/reports/tr15/ for details. */
                    UQ_WARNING << "Invalid shorcut key (should be one single character):" << value;
                }
            }
            if (!m_file->contains("ShowEntry")) {
                entry->setShowEntry(true);
            } else {
                entry->setShowEntry(m_file->value("ShowEntry").toBool());
            }
            m_file->endGroup();
            entry->setPosition(i++);
            connect(entry, SIGNAL(positionChanged(uint)),
                    SLOT(onEntryPositionChanged(uint)));
            m_static_entries[entry->dbusObjectPath()] = entry;
            int index = m_entries.size();
            beginInsertRows(QModelIndex(), index, index);
            m_entries.append(entry);
            endInsertRows();
        }

        /* Monitor the corresponding D-Bus place service */
        m_serviceWatcher->addWatchedService(m_dbusName);

        /* Connect to the live place immediately if the service is already running
           otherwise wait for around 10 seconds as to not impact startup time */
        QDBusConnectionInterface* iface = QDBusConnection::sessionBus().interface();
        QDBusReply<bool> registered = iface->isServiceRegistered(m_dbusName);
        if (registered) {
            onPlaceServiceRegistered();
        } else {
            QTimer::singleShot(10000, this, SLOT(connectToRemotePlace()));
        }
    } else {
        delete m_file;
        m_file = NULL;
        UQ_WARNING << "Invalid place file, missing [Place] group";
    }
}

QString
Place::dbusName() const
{
    return m_dbusName;
}

QString
Place::dbusObjectPath() const
{
    return m_dbusObjectPath;
}

bool
Place::online() const
{
    return m_online;
}

QVariant
Place::data(const QModelIndex& index, int role) const
{
    if (!index.isValid()) {
        return QVariant();
    }

    PlaceEntry* entry = m_entries.at(index.row());
    if (role == Place::RoleItem) {
        return QVariant::fromValue(entry);
    } else if (role == Place::RoleShowEntry) {
        return QVariant::fromValue(QString(entry->showEntry() ? "true" : "false"));
    } else {
        return QVariant();
    }
}

int
Place::rowCount(const QModelIndex& parent) const
{
    Q_UNUSED(parent)

    return m_entries.size();
}

void
Place::connectToRemotePlace()
{
    if ((m_dbusIface != NULL) && m_dbusIface->isValid()) {
        return;
    }

    if (m_dbusIface == NULL) {
        m_dbusIface = new QDBusInterface(m_dbusName, m_dbusObjectPath,
                                         UNITY_PLACE_INTERFACE);
    }

    QDBusConnection connection = m_dbusIface->connection();
    if (!connection.isConnected()) {
        UQ_WARNING << "ERROR: unable to connect to bus:"
                   << connection.lastError();
        return;
    }

    if (!m_dbusIface->isValid()) {
        /* A call to the interface will spawn the corresponding place daemon. */
        getEntries();
    }
}

void
Place::getEntries()
{
    if (m_querying) {
        /* A call to GetEntries is already pending. */
        return;
    }
    m_querying = true;

    /* Update the list of online entries. */
    QDBusPendingCall pcall = m_dbusIface->asyncCall("GetEntries");
    QDBusPendingCallWatcher* watcher = new QDBusPendingCallWatcher(pcall, this);
    connect(watcher, SIGNAL(finished(QDBusPendingCallWatcher*)),
            SLOT(gotEntries(QDBusPendingCallWatcher*)));
}

void
Place::startMonitoringEntries()
{
    QDBusConnection connection = m_dbusIface->connection();
    connection.connect(m_dbusName, m_dbusObjectPath, UNITY_PLACE_INTERFACE,
                       "EntryAdded", this, SLOT(onEntryAdded(const PlaceEntryInfoStruct&)));
    connection.connect(m_dbusName, m_dbusObjectPath, UNITY_PLACE_INTERFACE,
                       "EntryRemoved", this, SLOT(onEntryRemoved(const QString&)));
}

void
Place::stopMonitoringEntries()
{
    QDBusConnection connection = m_dbusIface->connection();
    connection.disconnect(m_dbusName, m_dbusObjectPath, UNITY_PLACE_INTERFACE,
                          "EntryAdded", this, SLOT(onEntryAdded(const PlaceEntryInfoStruct&)));
    connection.disconnect(m_dbusName, m_dbusObjectPath, UNITY_PLACE_INTERFACE,
                          "EntryRemoved", this, SLOT(onEntryRemoved(const QString&)));
}

void
Place::onPlaceServiceRegistered()
{
    connectToRemotePlace();

    m_online = true;
    Q_EMIT onlineChanged(m_online);

    startMonitoringEntries();
    getEntries();
}

void
Place::onPlaceServiceUnregistered()
{
    m_online = false;
    Q_EMIT onlineChanged(m_online);

    stopMonitoringEntries();

    for (int i = rowCount() - 1; i >= 0; --i) {
        PlaceEntry* entry = m_entries.at(i);
        if (!m_static_entries.contains(entry->dbusObjectPath())) {
            beginRemoveRows(QModelIndex(), i, i);
            m_entries.removeAt(i);
            endRemoveRows();
            delete entry;
        } else {
            entry->setSensitive(false);
        }
    }
}

void
Place::onEntryAdded(const PlaceEntryInfoStruct& p)
{
    PlaceEntry* entry = NULL;
    if (m_static_entries.contains(p.dbus_path)) {
        entry = m_static_entries.value(p.dbus_path);
    } else {
        entry = new PlaceEntry(this);
        entry->setDbusName(m_dbusName);
        entry->setDbusObjectPath(p.dbus_path);
    }

    entry->updateInfo(p);
    connect(entry, SIGNAL(positionChanged(uint)),
            SLOT(onEntryPositionChanged(uint)), Qt::UniqueConnection);
    entry->setSensitive(true);
    int index = m_entries.size();
    beginInsertRows(QModelIndex(), index, index);
    m_entries.append(entry);
    endInsertRows();
    entry->connectToRemotePlaceEntry();
    Q_EMIT entryAdded(entry);
}

void
Place::onEntryRemoved(const QString& dbusObjectPath)
{
    PlaceEntry* entry = NULL;
    QList<PlaceEntry*>::const_iterator i;
    for (i = m_entries.constBegin(); i != m_entries.constEnd(); ++i) {
        if ((*i)->dbusObjectPath() == dbusObjectPath) {
            entry = *i;
            break;
        }
    }

    if (entry != NULL) {
        Q_EMIT entryRemoved(entry);
        int index = m_entries.indexOf(entry);
        beginRemoveRows(QModelIndex(), index, index);
        m_entries.removeOne(entry);
        endRemoveRows();

        if (!m_static_entries.contains(entry->dbusObjectPath())) {
            delete entry;
        } else {
            entry->setSensitive(false);
        }
    }
}

void
Place::onEntryPositionChanged(uint position)
{
    /* This doesn’t seem to be implemented/used in Unity, but it can’t hurt… */
    // TODO: may require some sanity checks.
    PlaceEntry* entry = static_cast<PlaceEntry*>(sender());
    int from = m_entries.indexOf(entry);
    if (from != -1) {
        beginMoveRows(QModelIndex(), from, from, QModelIndex(), position);
        m_entries.move(from, position);
        endMoveRows();
    }
}

void
Place::gotEntries(QDBusPendingCallWatcher* watcher)
{
    QDBusPendingReply<QList<PlaceEntryInfoStruct> > reply = *watcher;
    if (reply.isError()) {
        UQ_WARNING << "ERROR:" << m_dbusName << reply.error().message();
        onPlaceServiceUnregistered();
    } else {
        QList<PlaceEntryInfoStruct> entries = reply.argumentAt<0>();
        QList<PlaceEntryInfoStruct>::const_iterator i;
        for (i = entries.constBegin(); i != entries.constEnd(); ++i) {
            if (m_static_entries.contains(i->dbus_path)) {
                PlaceEntry* entry = m_static_entries.value(i->dbus_path);
                entry->updateInfo(*i);
                if (m_entries.contains(entry)) {
                    entry->setSensitive(true);
                } else {
                    int index = m_entries.size();
                    beginInsertRows(QModelIndex(), index, index);
                    m_entries.append(entry);
                    endInsertRows();
                }
                entry->connectToRemotePlaceEntry();
            } else {
                onEntryAdded(*i);
            }
        }
    }
    watcher->deleteLater();
    m_querying = false;
}

PlaceEntry*
Place::findPlaceEntry(const QString& groupName)
{
    Q_FOREACH(PlaceEntry* entry, m_entries) {
        if (entry->groupName() == groupName) {
            return entry;
        }
    }

    return NULL;
}

void
Place::activate(QString uri)
{
    /* Tries various methods to trigger a sensible action for the given 'uri'.
       First it asks the place backend via its 'Activate' method. If that fails
       it does its best to select a relevant action for the uri's scheme. If it
       has no understanding of the given scheme it falls back on asking Qt to
       open the uri.
    */
    QUrl url(uri);
    if (url.scheme() == "file") {
        /* Override the files place's default URI handler: we want the file
           manager to handle opening folders, not the dash.

           Ref: https://bugs.launchpad.net/upicek/+bug/689667
        */
        QDesktopServices::openUrl(url);
        return;
    }

    QDBusInterface dbusActivationInterface(m_dbusName, m_dbusObjectPath,
                                           UNITY_ACTIVATION_INTERFACE);
    QDBusReply<uint> reply = dbusActivationInterface.call("Activate", uri);
    if (reply != 0) {
        return;
    }

    if (url.scheme() == "application") {
        LauncherApplication application;
        /* Cannot set the desktop file to url.host(), because the QUrl constructor
           converts the host name to lower case to conform to the Nameprep
           RFC (see http://doc.qt.nokia.com/qurl.html#FormattingOption-enum).
           Ref: https://bugs.launchpad.net/unity-place-applications/+bug/784478 */
        QString desktopFile = uri.mid(uri.indexOf("://") + 3);
        application.setDesktopFile(desktopFile);
        application.activate();
        return;
    }

    UQ_WARNING << "FIXME: Possibly no handler for scheme: " << url.scheme();
    UQ_WARNING << "Trying to open" << uri;
    /* Try our luck */
    QDesktopServices::openUrl(url);
}

#include "place.moc"
