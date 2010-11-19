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

#include "place.h"

#include <QStringList>
#include <QDebug>
#include <QDBusPendingReply>

static const char* UNITY_PLACE_INTERFACE = "com.canonical.Unity.Place";

Place::Place(QObject* parent) :
    QAbstractListModel(parent),
    m_file(NULL),
    m_online(false),
    m_dbusIface(NULL)
{
}

Place::Place(const Place &other)
{
    if (other.m_file != NULL)
        setFileName(other.m_file->fileName());
}

Place::~Place()
{
    delete m_dbusIface;
    delete m_file;
    QList<PlaceEntry*>::iterator iter;
    for(iter = m_entries.begin(); iter != m_entries.end(); ++iter)
    {
        delete *iter;
    }
    m_entries.clear();
}

QString
Place::fileName() const
{
    return m_file->fileName();
}

void
Place::setFileName(const QString &file)
{
    m_file = new QSettings(file, QSettings::IniFormat);
    if (m_file->childGroups().contains("Place"))
    {
        m_dbusName = m_file->value("Place/DBusName").toString();
        m_dbusObjectPath = m_file->value("Place/dbusObjectPath").toString();
        QStringList entries = m_file->childGroups().filter("Entry:");
        QStringList::const_iterator iter;
        uint i = 0;
        for(iter = entries.begin(); iter != entries.end(); ++iter)
        {
            PlaceEntry* entry = new PlaceEntry;
            entry->setFileName(file);
            entry->setGroupName(iter->mid(6));
            m_file->beginGroup(*iter);
            entry->setDbusName(m_dbusName);
            entry->setDbusObjectPath(m_file->value("DBusObjectPath").toString());
            // FIXME: extract localized name
            entry->setName(m_file->value("Name").toString());
            entry->setIcon(m_file->value("Icon").toString());
            m_file->endGroup();
            entry->setPosition(i);
            QObject::connect(entry, SIGNAL(positionChanged(uint)),
                             this, SLOT(onEntryPositionChanged(uint)));
            m_entries.append(entry);
            ++i;
        }
    }
    else
    {
        delete m_file;
        m_file = NULL;
        qWarning() << "Invalid place file, missing [Place] group";
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
    Q_UNUSED(role);

    if (!index.isValid())
        return QVariant();

    return QVariant::fromValue(m_entries.at(index.row()));
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
    if (m_online) return;

    m_dbusIface = new QDBusInterface(m_dbusName, m_dbusObjectPath,
                                     UNITY_PLACE_INTERFACE);
    if (!m_dbusIface->isValid()) {
        m_online = false;
        return;
    }

    // Connect to EntryAdded and EntryRemoved signals
    QDBusConnection connection = m_dbusIface->connection();
    connection.connect(m_dbusName, m_dbusObjectPath, UNITY_PLACE_INTERFACE,
                       "EntryAdded", this, SLOT(onEntryAdded(const PlaceEntryInfoStruct&)));
    connection.connect(m_dbusName, m_dbusObjectPath, UNITY_PLACE_INTERFACE,
                       "EntryRemoved", this, SLOT(onEntryRemoved(const QString&)));

    // Get the list of entries and update the existing entries.
    QDBusPendingCall pcall = m_dbusIface->asyncCall("GetEntries");
    QDBusPendingCallWatcher* watcher = new QDBusPendingCallWatcher(pcall, this);
    QObject::connect(watcher, SIGNAL(finished(QDBusPendingCallWatcher*)),
                     this, SLOT(gotEntries(QDBusPendingCallWatcher*)));

    m_online = true;
}

void
Place::onEntryAdded(const PlaceEntryInfoStruct& p)
{
    // TODO: test that this actually works… How do I add an entry on D-Bus?
    PlaceEntry* entry = new PlaceEntry;
    entry->setDbusName(m_dbusName);
    entry->setDbusObjectPath(p.dbus_path);
    entry->updateInfo(p);
    QObject::connect(entry, SIGNAL(positionChanged(uint)),
                     this, SLOT(onEntryPositionChanged(uint)));
    int index = m_entries.size();
    beginInsertRows(createIndex(0, 0), index, index);
    m_entries.append(entry);
    endInsertRows();
    entry->connectToRemotePlaceEntry();
    emit entryAdded(entry);
}

void
Place::onEntryRemoved(const QString& dbusObjectPath)
{
    // TODO: test that this actually works… How do I remove an entry on D-Bus?
    PlaceEntry* entry = NULL;
    QList<PlaceEntry*>::const_iterator i;
    for (i = m_entries.constBegin(); i != m_entries.constEnd(); ++i) {
        if ((*i)->dbusObjectPath() == dbusObjectPath) {
            entry = *i;
            break;
        }
    }
    if (entry != NULL) {
        emit entryRemoved(entry);
        int index = m_entries.indexOf(entry);
        beginRemoveRows(createIndex(0, 0), index, index);
        m_entries.removeOne(entry);
        endRemoveRows();
        delete entry;
    }
}

void
Place::onEntryPositionChanged(uint position)
{
    /* This doesn’t seem to be implemented/used in Unity, but it can’t hurt… */
    /* TODO: test that this actually works… How do I change the position of an
       entry on D-Bus? */
    // TODO: may require some sanity checks.
    PlaceEntry* entry = static_cast<PlaceEntry*>(sender());
    QModelIndex parent = createIndex(0, 0);
    int from = m_entries.indexOf(entry);
    beginMoveRows(parent, from, from, parent, position);
    m_entries.move(from, position);
    endMoveRows();
}

void
Place::gotEntries(QDBusPendingCallWatcher* watcher)
{
    QDBusPendingReply<QList<PlaceEntryInfoStruct> > reply = *watcher;
    if (reply.isError()) {
        qWarning() << "ERROR:" << m_dbusName << reply.error().message();
    } else {
        QList<PlaceEntryInfoStruct> entries = reply.argumentAt<0>();
        QList<PlaceEntryInfoStruct>::const_iterator i;
        for (i = entries.constBegin(); i != entries.constEnd(); ++i) {
            bool existing = false;
            QList<PlaceEntry*>::iterator j;
            for (j = m_entries.begin(); j != m_entries.end(); ++j) {
                PlaceEntry* entry = *j;
                if (entry->dbusObjectPath() == i->dbus_path) {
                    entry->updateInfo(*i);
                    entry->connectToRemotePlaceEntry();
                    existing = true;
                }
            }
            if (!existing) {
                onEntryAdded(*i);
            }
        }

        /* Now remove those that couldn’t connect or did not exist in the live
           place. */
        for (int k = m_entries.size() - 1; k != -1; --k) {
            PlaceEntry* entry = m_entries.at(k);
            if (!entry->online()) {
                emit entryRemoved(entry);
                beginRemoveRows(createIndex(0, 0), k, k);
                m_entries.removeAt(k);
                endRemoveRows();
                delete entry;
            }
        }
    }
    watcher->deleteLater();
}

