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
    Q_INVOKABLE int rowCount(const QModelIndex& parent = QModelIndex()) const;
    Q_INVOKABLE QVariant get(int row) const;

    /* This method is the QML equivalent of aggregateListModel.
       The reason why aggregateListModel wasn't directly exposed to QML is that
       QAbstractListModel can't be exposed to QML directly since it's an abstract
       class. Therefore we accept a QVariant here and internally cast it back
       to a QAbstractListModel */
    Q_INVOKABLE void appendModel(const QVariant& model);

    /* Move one item from one position to another position.
       The item must remain in the same model. */
    Q_INVOKABLE void move(int from, int to);

protected:
    QList<QAbstractListModel*> m_models;

    void aggregateListModel(QAbstractListModel* model);
    void removeListModel(QAbstractListModel* model);

private slots:
    void onRowsInserted(const QModelIndex& parent, int first, int last);
    void onRowsRemoved(const QModelIndex& parent, int first, int last);
    void onRowsMoved(const QModelIndex&, int, int, const QModelIndex&, int);

private:
    int computeOffset(QAbstractListModel* model) const;
    QAbstractListModel* modelAtIndex(int index) const;
};

#endif // LISTAGGREGATORMODEL_H

