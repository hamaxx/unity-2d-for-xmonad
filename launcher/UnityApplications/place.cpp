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

Place::Place(QObject* parent) :
    QAbstractListModel(parent), m_file(NULL)
{
}

Place::Place(const Place &other)
{
    if (other.m_file != NULL)
        setFileName(other.m_file->fileName());
}

Place::~Place()
{
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
        m_dbusObjectName = m_file->value("Place/dbusObjectPath").toString();
        QStringList entries = m_file->childGroups().filter("Entry:");
        QStringList::const_iterator iter;
        for(iter = entries.begin(); iter != entries.end(); ++iter)
        {
            PlaceEntry* entry = new PlaceEntry;
            entry->setFileName(file);
            entry->setGroupName(iter->mid(6));
            m_file->beginGroup(*iter);
            entry->setDbusObjectPath(m_file->value("DBusObjectPath").toString());
            // FIXME: extract localized name
            entry->setName(m_file->value("Name").toString());
            entry->setIcon(m_file->value("Icon").toString());
            m_file->endGroup();
            m_entries.append(entry);
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
    return m_dbusObjectName;
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
