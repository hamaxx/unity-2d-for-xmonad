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

#ifndef LISTAGGREGATORMODEL_H
#define LISTAGGREGATORMODEL_H

#include <QAbstractListModel>

class ListAggregatorModel : public QAbstractListModel
{
    Q_OBJECT

public:
    ListAggregatorModel(QObject* parent = 0);
    ~ListAggregatorModel();

    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;
    int rowCount(const QModelIndex& parent = QModelIndex()) const;

protected:
    QList<QAbstractListModel*> m_models;

    void aggregateListModel(QAbstractListModel* model);
    void removeListModel(QAbstractListModel* model);

private slots:
    void onRowsInserted(const QModelIndex& parent, int first, int last);
    void onRowsRemoved(const QModelIndex& parent, int first, int last);

private:
    int computeOffset(QAbstractListModel* model) const;
};

#endif // LISTAGGREGATORMODEL_H

