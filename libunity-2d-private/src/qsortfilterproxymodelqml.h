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

#ifndef QSORTFILTERPROXYMODELQML_H
#define QSORTFILTERPROXYMODELQML_H

#include <QSortFilterProxyModel>

class QSortFilterProxyModelQML : public QSortFilterProxyModel
{
    Q_OBJECT

    Q_PROPERTY(QObject* model READ sourceModelQObject WRITE setSourceModelQObject)
    Q_PROPERTY(int limit READ limit WRITE setLimit NOTIFY limitChanged)
    Q_PROPERTY(int totalCount READ totalCount NOTIFY totalCountChanged)
    Q_PROPERTY(int count READ count NOTIFY countChanged)

public:
    explicit QSortFilterProxyModelQML(QObject *parent = 0);

    Q_INVOKABLE QVariantMap get(int row);
    Q_INVOKABLE int count();
    int rowCount(const QModelIndex &parent = QModelIndex()) const;

    /* getters */
    QObject* sourceModelQObject() const;
    int limit() const;
    int totalCount() const;

    /* setters */
    void setSourceModelQObject(QObject *model);
    void setLimit(int limit);

    Q_SLOT void setRoleNames(const QHash<int,QByteArray> &roleNames);

Q_SIGNALS:
    void limitChanged();
    void totalCountChanged();
    void countChanged();
    void roleNamesChanged(const QHash<int,QByteArray> &);

private:
    int m_limit;
};

#endif // QSORTFILTERPROXYMODELQML_H