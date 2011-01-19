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

    /* FIXME: This property is only here as a workaround for the following Qt bug:
              http://bugreports.qt.nokia.com/browse/QTBUG-12905
              We need it so we can be notified of changes in the number of items,
              but we should really use Repeater.count (in spread/Spread.qml)
    */
    Q_PROPERTY(int count READ count NOTIFY countChanged);

public:
    explicit QSortFilterProxyModelQML(QObject *parent = 0);

    /* getters */
    QObject* sourceModelQObject() const;
    int count() const { return rowCount(); }

    /* setters */
    void setSourceModelQObject(QObject *model);

Q_SIGNALS:
    void countChanged(int count);

public slots:
    void updateCount();

private slots:
    void updateRoleNames();
};

#endif // QSORTFILTERPROXYMODELQML_H
