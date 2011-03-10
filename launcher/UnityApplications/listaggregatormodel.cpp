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

#include "listaggregatormodel.h"

#include <QDebug>

ListAggregatorModel::ListAggregatorModel(QObject* parent) :
    QAbstractListModel(parent)
{
    QHash<int, QByteArray> roles;
    roles[0] = "item";
    setRoleNames(roles);
}

ListAggregatorModel::~ListAggregatorModel()
{
}

void
ListAggregatorModel::appendModel(const QVariant& model)
{
    QObject* object = qvariant_cast<QObject*>(model);
    QAbstractListModel* list = qobject_cast<QAbstractListModel*>(object);
    aggregateListModel(list);
}

void
ListAggregatorModel::aggregateListModel(QAbstractListModel* model)
{
    if (model == NULL) {
        return;
    }

    int modelRowCount = model->rowCount();
    if (modelRowCount > 0) {
        int first = rowCount();
        int last = first + modelRowCount - 1;
        beginInsertRows(QModelIndex(), first, last);
    }

    m_models.append(model);
    if (modelRowCount > 0) {
        endInsertRows();
    }

    connect(model, SIGNAL(rowsInserted(const QModelIndex&, int, int)),
            SLOT(onRowsInserted(const QModelIndex&, int, int)));
    connect(model, SIGNAL(rowsRemoved(const QModelIndex&, int, int)),
            SLOT(onRowsRemoved(const QModelIndex&, int, int)));
    connect(model, SIGNAL(rowsMoved(const QModelIndex&, int, int, const QModelIndex&, int)),
            SLOT(onRowsMoved(const QModelIndex&, int, int, const QModelIndex&, int)));
}

void
ListAggregatorModel::removeListModel(QAbstractListModel* model)
{
    int modelRowCount = model->rowCount();
    if (modelRowCount > 0) {
        int first = computeOffset(model);
        int last = first + modelRowCount - 1;
        beginRemoveRows(QModelIndex(), first, last);
    }

    m_models.removeOne(model);
    if (modelRowCount > 0) {
        endRemoveRows();
    }

    disconnect(model, SIGNAL(rowsInserted(const QModelIndex&, int, int)),
               this, SLOT(onRowsInserted(const QModelIndex&, int, int)));
    disconnect(model, SIGNAL(rowsRemoved(const QModelIndex&, int, int)),
               this, SLOT(onRowsRemoved(const QModelIndex&, int, int)));
    disconnect(model, SIGNAL(rowsMoved(const QModelIndex&, int, int, const QModelIndex&, int)),
               this, SLOT(onRowsMoved(const QModelIndex&, int, int, const QModelIndex&, int)));
}

void
ListAggregatorModel::move(int from, int to)
{
    QAbstractListModel* model = modelAtIndex(from);
    if (modelAtIndex(to) != model) {
        qWarning() << "cannot move an item from one model to another";
        return;
    }

    int offset = computeOffset(model);
    // "move" is not a member of QAbstractListModel, cannot be invoked directly
    QMetaObject::invokeMethod(model, "move",
                              Q_ARG(int, from - offset),
                              Q_ARG(int, to - offset));
}

int
ListAggregatorModel::computeOffset(QAbstractListModel* model) const
{
    int offset = 0;
    QList<QAbstractListModel*>::const_iterator iter;
    for (iter = m_models.begin(); (iter != m_models.end()) && (*iter != model); ++iter) {
        offset += (*iter)->rowCount();
    }
    return offset;
}

QAbstractListModel*
ListAggregatorModel::modelAtIndex(int index) const
{
    int offset = index;
    Q_FOREACH(QAbstractListModel* model, m_models) {
        int size = model->rowCount();
        if (offset < size) {
            return model;
        }
        offset -= size;
    }
    return NULL;
}

void
ListAggregatorModel::onRowsInserted(const QModelIndex& parent, int first, int last)
{
    QAbstractListModel* model = static_cast<QAbstractListModel*>(sender());
    int offset = computeOffset(model);
    beginInsertRows(parent, first + offset, last + offset);
    endInsertRows();
}

void
ListAggregatorModel::onRowsRemoved(const QModelIndex& parent, int first, int last)
{
    QAbstractListModel* model = static_cast<QAbstractListModel*>(sender());
    int offset = computeOffset(model);
    beginRemoveRows(parent, first + offset, last + offset);
    endRemoveRows();
}

void
ListAggregatorModel::onRowsMoved(const QModelIndex& sourceParent, int sourceStart, int sourceEnd,
                                 const QModelIndex& destinationParent, int destinationRow)
{
    QAbstractListModel* model = static_cast<QAbstractListModel*>(sender());
    int offset = computeOffset(model);
    beginMoveRows(sourceParent, sourceStart + offset, sourceEnd + offset,
                  destinationParent, destinationRow + offset);
    endMoveRows();
}

int
ListAggregatorModel::rowCount(const QModelIndex& parent) const
{
    Q_UNUSED(parent)

    int count = 0;
    QList<QAbstractListModel*>::const_iterator iter;
    for (iter = m_models.begin(); iter != m_models.end(); ++iter) {
        count += (*iter)->rowCount();
    }
    return count;
}

QVariant
ListAggregatorModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid()) {
        return QVariant();
    }

    int row = index.row();
    int offset = row;
    QList<QAbstractListModel*>::const_iterator iter;
    for (iter = m_models.begin(); iter != m_models.end(); ++iter) {
        int rowCount = (*iter)->rowCount();
        if (offset >= rowCount) {
            offset -= rowCount;
        } else {
            QModelIndex new_index = createIndex(offset, role);
            return (*iter)->data(new_index, role);
        }
    }

    // For the sake of completeness, should never happen.
    return QVariant();
}

QVariant
ListAggregatorModel::get(int row) const
{
    return data(QAbstractListModel::index(row), 0);
}
