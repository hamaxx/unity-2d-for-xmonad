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
    QAbstractListModel(parent), m_file(NULL), m_dbusIface(NULL)
{
}

Place::Place(const Place &other)
{
    if (other.m_file != NULL)
        setFileName(other.m_file->fileName());
    // TODO: connect()
}

Place::~Place()
{
    if (m_dbusIface != NULL)
    {
        // TODO: disconnect()
        delete m_dbusIface;
    }
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
    m_dbusIface = new QDBusInterface(m_dbusName, m_dbusObjectPath,
                                     UNITY_PLACE_INTERFACE);

    // Connect to EntryAdded and EntryRemoved signals
    QDBusConnection connection = m_dbusIface->connection();
    connection.connect(m_dbusName, m_dbusObjectPath, UNITY_PLACE_INTERFACE,
                       "EntryAdded", this, SLOT(onEntryAdded(const PlaceEntryInfoStruct&)));
    connection.connect(m_dbusName, m_dbusObjectPath, UNITY_PLACE_INTERFACE,
                       "EntryRemoved", this, SLOT(onEntryRemoved()));

    // Get the list of entries and update the existing entries.
    // See unity/unity-private/places/places-place.vala:167
    QDBusPendingCall pcall = m_dbusIface->asyncCall("GetEntries");
    QDBusPendingCallWatcher* watcher = new QDBusPendingCallWatcher(pcall, this);
    QObject::connect(watcher, SIGNAL(finished(QDBusPendingCallWatcher*)),
                     this, SLOT(gotEntries(QDBusPendingCallWatcher*)));
}

void
Place::onEntryAdded(const PlaceEntryInfoStruct& p)
{
    qDebug() << "onEntryAdded";
    // TODO
}

void
Place::onEntryRemoved(QVariant blah)
{
    qDebug() << "onEntryRemoved:" << blah;
    // TODO
}

void
Place::onEntryPositionChanged(uint position)
{
    PlaceEntry* entry = static_cast<PlaceEntry*>(sender());
    qDebug() << "onEntryPositionChanged(" << entry << ", " << position << ")";
    // TODO
}

void
Place::gotEntries(QDBusPendingCallWatcher* watcher)
{
    qDebug() << "Place::gotEntries(" << watcher << ")";

    QDBusPendingReply<QList<PlaceEntryInfoStruct> > reply = *watcher;
    if (reply.isError()) {
        qWarning() << reply.error().message();
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
        // TODO (see unity-private/places/places-place.vala:193)
    }
    watcher->deleteLater();
}

