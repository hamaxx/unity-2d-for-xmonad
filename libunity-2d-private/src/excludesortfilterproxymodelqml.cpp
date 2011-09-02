/*
 * Copyright (C) 2011 Canonical, Ltd.
 *
 * Authors:
 *  Ugo Riboni <ugo.riboni@canonical.com>
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

#include "excludesortfilterproxymodelqml.h"

ExcludeSortFilterProxyModelQML::ExcludeSortFilterProxyModelQML(QObject *parent) :
    QSortFilterProxyModelQML(parent)
{
}

#include <QDebug>

bool ExcludeSortFilterProxyModelQML::filterAcceptsRow(int sourceRow,
                                                      const QModelIndex &sourceParent) const
{
    // If there's no regexp set, keep default behaviour of accepting all rows.
    if (filterRegExp().isEmpty()) {
        return true;
    }

    bool accept = QSortFilterProxyModelQML::filterAcceptsRow(sourceRow, sourceParent);
    return !accept;

    //return !QSortFilterProxyModelQML::filterAcceptsRow(sourceRow, sourceParent);
}

#include "excludesortfilterproxymodelqml.moc"
