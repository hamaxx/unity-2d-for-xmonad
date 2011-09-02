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

#ifndef EXCLUDESORTFILTERPROXYMODELQML_H
#define EXCLUDESORTFILTERPROXYMODELQML_H

#include "qsortfilterproxymodelqml.h"

#include <QModelIndex>
#include <QObject>

class ExcludeSortFilterProxyModelQML : public QSortFilterProxyModelQML
{
    Q_OBJECT

public:
    explicit ExcludeSortFilterProxyModelQML(QObject *parent = 0);

    virtual bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const;
};

#endif // EXCLUDESORTFILTERPROXYMODELQML_H
