/*
 * Copyright (C) 2011 Canonical, Ltd.
 *
 * Authors:
 *  Florian Boucault <florian.boucault@canonical.com>
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

#ifndef LENSES_H
#define LENSES_H

// Qt
#include <QAbstractListModel>

namespace unity
{
namespace dash
{
class Lenses;
}
}

class Lenses : public QAbstractListModel
{
    Q_OBJECT
public:
    explicit Lenses(QObject *parent = 0);
    ~Lenses();

    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;
    int rowCount(const QModelIndex& parent = QModelIndex()) const;

private:
    unity::dash::Lenses* m_unityLenses;
};

#endif // LENSES_H
