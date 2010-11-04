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
#include "placeentry.h"

#include <QStringList>

Place::Place(const QString& file)
{
    m_file = new QSettings(file, QSettings::IniFormat);
    QStringList groups = m_file->childGroups();
}

Place::~Place()
{
    delete m_file;
}

QString
Place::file() const
{
    return m_file->fileName();
}

QString
Place::dbusName() const
{
    return m_file->value("Place/DBusName").toString();
}

QString
Place::dbusObjectPath() const
{
    return m_file->value("Place/dbusObjectPath").toString();
}

QVariant
Place::data(const QModelIndex& index, int role) const
{
    Q_UNUSED(role);

    if (!index.isValid())
        return QVariant();

    QString group = m_file->childGroups().filter("Entry:").at(index.row());
    PlaceEntry* entry = new PlaceEntry;
    entry->setGroupName(group.mid(6));
    m_file->beginGroup(group);
    entry->setDbusObjectPath(m_file->value("DBusObjectPath").toString());
    // FIXME: extract localized name
    entry->setName(m_file->value("Name").toString());
    entry->setIcon(m_file->value("Icon").toString());
    m_file->endGroup();

    return QVariant::fromValue(entry);
}

int
Place::rowCount(const QModelIndex& parent) const
{
    Q_UNUSED(parent)

    return m_file->childGroups().filter("Entry:").size();
}

