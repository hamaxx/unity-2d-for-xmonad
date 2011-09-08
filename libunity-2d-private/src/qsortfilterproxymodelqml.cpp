/*
 * Copyright (C) 2010 Canonical, Ltd.
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

#include "qsortfilterproxymodelqml.h"
#include <debug_p.h>

QSortFilterProxyModelQML::QSortFilterProxyModelQML(QObject *parent)
    : QSortFilterProxyModel(parent)
    , m_limit(-1)
    , m_invertMatch(false)
{
    connect(this, SIGNAL(modelReset()), SIGNAL(countChanged()));
    connect(this, SIGNAL(rowsInserted(QModelIndex,int,int)), SIGNAL(countChanged()));
    connect(this, SIGNAL(rowsRemoved(QModelIndex,int,int)), SIGNAL(countChanged()));
}

void QSortFilterProxyModelQML::setRoleNames(const QHash<int,QByteArray> &roleNames)
{
    QSortFilterProxyModel::setRoleNames(roleNames);
    Q_EMIT roleNamesChanged(roleNames);
}

QObject*
QSortFilterProxyModelQML::sourceModelQObject() const
{
    return (QAbstractItemModel*)sourceModel();
}

void
QSortFilterProxyModelQML::setSourceModelQObject(QObject *model)
{
    if (model == NULL) {
        return;
    }

    QAbstractItemModel* itemModel = qobject_cast<QAbstractItemModel*>(model);
    if (itemModel == NULL) {
        UQ_WARNING << "QSortFilterProxyModelQML only accepts objects of type QAbstractItemModel as its model";
        return;
    }

    if (sourceModel() != NULL) {
        sourceModel()->disconnect(this);
    }

    /* Workaround for limitation of QAbstractProxyModel: if sourceModel's
       roleNames changes, the QAbstractProxyModel's roleNames are not updated
       to reflect that change.
       As a consequence it works around Qt bug http://bugreports.qt.nokia.com/browse/QTBUG-20405
    */
    bool hasRoleNamesChangedSignal;
    hasRoleNamesChangedSignal = connect(itemModel, SIGNAL(roleNamesChanged(QHash<int,QByteArray>)), SLOT(setRoleNames(QHash<int,QByteArray>)));
    if (!hasRoleNamesChangedSignal) {
        UQ_WARNING << "received a sourceModel that does not notify of changes of its roleNames";
    }
    setRoleNames(itemModel->roleNames());

    setSourceModel(itemModel);

    connect(itemModel, SIGNAL(modelReset()), SIGNAL(totalCountChanged()));
    connect(itemModel, SIGNAL(rowsInserted(QModelIndex,int,int)), SIGNAL(totalCountChanged()));
    connect(itemModel, SIGNAL(rowsRemoved(QModelIndex,int,int)), SIGNAL(totalCountChanged()));
    Q_EMIT totalCountChanged();
    Q_EMIT countChanged();
}

QVariantMap
QSortFilterProxyModelQML::get(int row)
{
    if (sourceModel() == NULL) {
        return QVariantMap();
    }

    QVariantMap result;
    QHashIterator<int, QByteArray> i(roleNames());
    while (i.hasNext()) {
        i.next();
        QModelIndex modelIndex = index(row, 0);
        QVariant data = modelIndex.data(i.key());
        result[i.value()] = data;
     }
     return result;
}

int
QSortFilterProxyModelQML::totalCount() const
{
    if (sourceModel() != NULL) {
        return sourceModel()->rowCount();
    } else {
        return 0;
    }
}

int
QSortFilterProxyModelQML::count()
{
    return rowCount();
}

int
QSortFilterProxyModelQML::limit() const
{
    return m_limit;
}

void
QSortFilterProxyModelQML::setLimit(int limit)
{
    if (limit != -1 && !filterRegExp().isEmpty()) {
        qFatal("QSortFilterProxyModel: filterRegExp and limit are both set which is not supported");
    }

    if (limit != m_limit) {
        m_limit = limit;
        invalidateFilter();
        Q_EMIT limitChanged();
    }
}

bool
QSortFilterProxyModelQML::invertMatch() const
{
    return m_invertMatch;
}

void
QSortFilterProxyModelQML::setInvertMatch(bool invertMatch)
{
    if (invertMatch != m_invertMatch) {
        m_invertMatch = invertMatch;
        Q_EMIT invertMatchChanged(invertMatch);
    }
}

bool
QSortFilterProxyModelQML::filterAcceptsRow(int sourceRow,
                                           const QModelIndex &sourceParent) const
{
    if (m_limit != -1 && sourceRow >= m_limit) {
        return false;
    }
    // If there's no regexp set, always accept all rows indepenently of the invertMatch setting
    if (filterRegExp().isEmpty()) {
        return true;
    }

    bool result = QSortFilterProxyModel::filterAcceptsRow(sourceRow, sourceParent);
    return (m_invertMatch) ? !result : result;
}

#include "qsortfilterproxymodelqml.moc"
