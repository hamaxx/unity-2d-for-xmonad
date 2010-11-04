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

#ifndef PLACE_H
#define PLACE_H

#include <QAbstractListModel>
#include <QString>
#include <QSettings>

class Place : public QAbstractListModel
{
    Q_OBJECT

    Q_PROPERTY(QString dbusName READ dbusName)
    Q_PROPERTY(QString dbusObjectPath READ dbusObjectPath)

public:
    Place(const QString& file);
    ~Place();

    /* getters */
    QString file() const;
    QString dbusName() const;
    QString dbusObjectPath() const;

    /* methods */
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;
    int rowCount(const QModelIndex& parent = QModelIndex()) const;

private:
    QSettings* m_file;
};

#endif // PLACE_H

