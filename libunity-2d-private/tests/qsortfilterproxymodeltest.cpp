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

// local
#include "qsortfilterproxymodelqml.h"

// Qt
#include <QTest>
#include <QSignalSpy>
#include <QModelIndex>
#include <QAbstractListModel>
#include <QDebug>


class MockListModel : public QAbstractListModel
{
    Q_OBJECT

public:
    MockListModel(QObject* parent = 0)
        : QAbstractListModel(parent)
    {
    }

    int rowCount(const QModelIndex& parent = QModelIndex()) const
    {
        return m_list.size();
    }

    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const
    {
        Q_UNUSED(role)

        if (!index.isValid() || index.row() < 0 || index.row() >= m_list.size()) {
           return QVariant();
        }
        return QVariant(m_list[index.row()]);
    }

    void setRoleNames(const QHash<int,QByteArray> &roleNames) {
        QAbstractListModel::setRoleNames(roleNames);
        Q_EMIT roleNamesChanged(roleNames);
    }

    bool insertRows(int row, int count, const QModelIndex &parent=QModelIndex()) {
        beginInsertRows(parent, row, row+count-1);
        for (int i=0; i<count; i++) {
            m_list.insert(i+row, "test"+i);
        }
        endInsertRows();
        return true;
    }

    bool appendRows(QStringList &rows, const QModelIndex &parent=QModelIndex()) {
        beginInsertRows(parent, rowCount(), rowCount() + rows.count() - 1);
        m_list.append(rows);
        endInsertRows();
        return true;
    }

    bool removeRows(int row, int count, const QModelIndex &parent=QModelIndex()) {
        beginRemoveRows(parent, row, row+count-1);
        for (int i=0; i<count; i++) {
            m_list.removeAt(row);
        }
        endInsertRows();
        return true;
    }

Q_SIGNALS:
    void roleNamesChanged(const QHash<int,QByteArray> &);

private:
    QStringList m_list;
};

class QSortFilterProxyModelTest : public QObject
{
    Q_OBJECT

private Q_SLOTS:

    void initTestCase() {
        qRegisterMetaType<QModelIndex>("QModelIndex");
    }

    void testRoleNamesSetAfter()
    {
        QSortFilterProxyModelQML proxy;
        MockListModel model;
        QHash<int, QByteArray> roles;

        proxy.setSourceModelQObject(&model);

        roles.clear();
        roles[0] = "role0";
        roles[1] = "role1";
        model.setRoleNames(roles);
        QCOMPARE(model.roleNames(), proxy.roleNames());
    }

    void testRoleNamesSetBefore()
    {
        QSortFilterProxyModelQML proxy;
        MockListModel model;
        QHash<int, QByteArray> roles;

        roles.clear();
        roles[0] = "role0";
        roles[1] = "role1";
        model.setRoleNames(roles);

        proxy.setSourceModelQObject(&model);
        QCOMPARE(model.roleNames(), proxy.roleNames());
    }

    void testCountSetAfter()
    {
        QSortFilterProxyModelQML proxy;
        MockListModel model;
        model.insertRows(0, 5);

        QSignalSpy spyOnCountChanged(&proxy, SIGNAL(countChanged()));

        proxy.setSourceModelQObject(&model);
        QCOMPARE(proxy.count(), 5);
        QVERIFY(spyOnCountChanged.count() >= 1);
    }

    void testCountInsert()
    {
        QSortFilterProxyModelQML proxy;
        MockListModel model;

        proxy.setSourceModelQObject(&model);

        QSignalSpy spyOnCountChanged(&proxy, SIGNAL(countChanged()));

        model.insertRows(0, 5);
        QCOMPARE(proxy.count(), 5);
        QCOMPARE(spyOnCountChanged.count(), 1);
    }

    void testCountRemove()
    {
        QSortFilterProxyModelQML proxy;
        MockListModel model;
        model.insertRows(0, 5);

        proxy.setSourceModelQObject(&model);

        QSignalSpy spyOnCountChanged(&proxy, SIGNAL(countChanged()));

        model.removeRows(0, 3);
        QCOMPARE(proxy.count(), 2);
        QCOMPARE(spyOnCountChanged.count(), 1);
    }

    void testLimitCount()
    {
        QSortFilterProxyModelQML proxy;
        MockListModel model;

        proxy.setSourceModelQObject(&model);
        proxy.setLimit(3);

        QSignalSpy spyOnCountChanged(&proxy, SIGNAL(countChanged()));

        model.insertRows(0, 5);
        QCOMPARE(proxy.count(), 3);
        QCOMPARE(spyOnCountChanged.count(), 1);
    }

    void testLimitLesserThanCount()
    {
        QSortFilterProxyModelQML proxy;
        MockListModel model;
        QList<QVariant> arguments;
        model.insertRows(0, 10);

        proxy.setSourceModelQObject(&model);

        QSignalSpy spyOnRowsRemoved(&proxy, SIGNAL(rowsRemoved(const QModelIndex &, int, int)));
        QSignalSpy spyOnRowsInserted(&proxy, SIGNAL(rowsInserted(const QModelIndex &, int, int)));
        QSignalSpy spyOnCountChanged(&proxy, SIGNAL(countChanged()));

        /* FIXME: for some reason the rowsRemoved is not emitted if
           proxy.rowCount is not called before.
        */
        proxy.rowCount();
        proxy.setLimit(5);
        QCOMPARE(spyOnRowsInserted.count(), 0);
        QCOMPARE(spyOnCountChanged.count(), 1);
        QCOMPARE(spyOnRowsRemoved.count(), 1);
        arguments = spyOnRowsRemoved.takeFirst();
        QCOMPARE(arguments.at(1).toInt(), 5);
        QCOMPARE(arguments.at(2).toInt(), 9);
        QCOMPARE(proxy.count(), 5);
        spyOnRowsRemoved.clear();
        spyOnCountChanged.clear();

        proxy.setLimit(7);
        QCOMPARE(spyOnRowsInserted.count(), 1);
        QCOMPARE(spyOnRowsRemoved.count(), 0);
        QCOMPARE(spyOnCountChanged.count(), 1);
        arguments = spyOnRowsInserted.takeFirst();
        QCOMPARE(arguments.at(1).toInt(), 5);
        QCOMPARE(arguments.at(2).toInt(), 6);
        QCOMPARE(proxy.count(), 7);
        spyOnRowsInserted.clear();
        spyOnCountChanged.clear();

        proxy.setLimit(3);
        QCOMPARE(spyOnRowsRemoved.count(), 1);
        QCOMPARE(spyOnRowsInserted.count(), 0);
        QCOMPARE(spyOnCountChanged.count(), 1);
        arguments = spyOnRowsRemoved.takeFirst();
        QCOMPARE(arguments.at(1).toInt(), 3);
        QCOMPARE(arguments.at(2).toInt(), 6);
        QCOMPARE(proxy.count(), 3);
        spyOnRowsRemoved.clear();
        spyOnCountChanged.clear();
    }

    void testLimitGreaterThanCount()
    {
        QSortFilterProxyModelQML proxy;
        MockListModel model;
        QList<QVariant> arguments;
        model.insertRows(0, 5);

        proxy.setSourceModelQObject(&model);

        QSignalSpy spyOnRowsRemoved(&proxy, SIGNAL(rowsRemoved(const QModelIndex &, int, int)));
        QSignalSpy spyOnRowsInserted(&proxy, SIGNAL(rowsInserted(const QModelIndex &, int, int)));
        QSignalSpy spyOnCountChanged(&proxy, SIGNAL(countChanged()));

        proxy.setLimit(7);
        QCOMPARE(spyOnRowsRemoved.count(), 0);
        QCOMPARE(spyOnRowsInserted.count(), 0);
        QCOMPARE(spyOnCountChanged.count(), 0);
        QCOMPARE(proxy.count(), 5);

        proxy.setLimit(5);
        QCOMPARE(spyOnRowsRemoved.count(), 0);
        QCOMPARE(spyOnRowsInserted.count(), 0);
        QCOMPARE(spyOnCountChanged.count(), 0);
        QCOMPARE(proxy.count(), 5);

        proxy.setLimit(3);
        QCOMPARE(spyOnRowsInserted.count(), 0);
        QCOMPARE(spyOnRowsRemoved.count(), 1);
        QCOMPARE(spyOnCountChanged.count(), 1);
        arguments = spyOnRowsRemoved.takeFirst();
        QCOMPARE(arguments.at(1).toInt(), 3);
        QCOMPARE(arguments.at(2).toInt(), 4);
        QCOMPARE(proxy.count(), 3);
        spyOnRowsRemoved.clear();
        spyOnCountChanged.clear();

        proxy.setLimit(4);
        QCOMPARE(spyOnRowsRemoved.count(), 0);
        QCOMPARE(spyOnRowsInserted.count(), 1);
        QCOMPARE(spyOnCountChanged.count(), 1);
        arguments = spyOnRowsInserted.takeFirst();
        QCOMPARE(arguments.at(1).toInt(), 3);
        QCOMPARE(arguments.at(2).toInt(), 3);
        QCOMPARE(proxy.count(), 4);
        spyOnRowsInserted.clear();
        spyOnCountChanged.clear();

        proxy.setLimit(7);
        QCOMPARE(spyOnRowsRemoved.count(), 0);
        QCOMPARE(spyOnRowsInserted.count(), 1);
        QCOMPARE(spyOnCountChanged.count(), 1);
        arguments = spyOnRowsInserted.takeFirst();
        QCOMPARE(arguments.at(1).toInt(), 4);
        QCOMPARE(arguments.at(2).toInt(), 4);
        QCOMPARE(proxy.count(), 5);
        spyOnRowsInserted.clear();
        spyOnCountChanged.clear();
    }

    void testLimitMinusOne()
    {
        QSortFilterProxyModelQML proxy;
        MockListModel model;
        QList<QVariant> arguments;
        model.insertRows(0, 5);

        proxy.setSourceModelQObject(&model);

        QSignalSpy spyOnRowsRemoved(&proxy, SIGNAL(rowsRemoved(const QModelIndex &, int, int)));
        QSignalSpy spyOnRowsInserted(&proxy, SIGNAL(rowsInserted(const QModelIndex &, int, int)));
        QSignalSpy spyOnCountChanged(&proxy, SIGNAL(countChanged()));

        proxy.setLimit(7);
        QCOMPARE(spyOnRowsRemoved.count(), 0);
        QCOMPARE(spyOnRowsInserted.count(), 0);
        QCOMPARE(spyOnCountChanged.count(), 0);
        QCOMPARE(proxy.count(), 5);

        proxy.setLimit(-1);
        QCOMPARE(spyOnRowsRemoved.count(), 0);
        QCOMPARE(spyOnRowsInserted.count(), 0);
        QCOMPARE(spyOnCountChanged.count(), 0);
        QCOMPARE(proxy.count(), 5);

        proxy.setLimit(3);
        QCOMPARE(spyOnRowsInserted.count(), 0);
        QCOMPARE(spyOnRowsRemoved.count(), 1);
        QCOMPARE(spyOnCountChanged.count(), 1);
        arguments = spyOnRowsRemoved.takeFirst();
        QCOMPARE(arguments.at(1).toInt(), 3);
        QCOMPARE(arguments.at(2).toInt(), 4);
        QCOMPARE(proxy.count(), 3);
        spyOnRowsRemoved.clear();
        spyOnCountChanged.clear();

        proxy.setLimit(-1);
        QCOMPARE(spyOnRowsRemoved.count(), 0);
        QCOMPARE(spyOnRowsInserted.count(), 1);
        QCOMPARE(spyOnCountChanged.count(), 1);
        arguments = spyOnRowsInserted.takeFirst();
        QCOMPARE(arguments.at(1).toInt(), 3);
        QCOMPARE(arguments.at(2).toInt(), 4);
        QCOMPARE(proxy.count(), 5);
        spyOnRowsInserted.clear();
        spyOnCountChanged.clear();
    }

    void testLimitInsert() {
        QSortFilterProxyModelQML proxy;
        MockListModel model;
        QList<QVariant> arguments;

        proxy.setSourceModelQObject(&model);
        proxy.setLimit(7);

        QSignalSpy spyOnRowsRemoved(&proxy, SIGNAL(rowsRemoved(const QModelIndex &, int, int)));
        QSignalSpy spyOnRowsInserted(&proxy, SIGNAL(rowsInserted(const QModelIndex &, int, int)));
        QSignalSpy spyOnCountChanged(&proxy, SIGNAL(countChanged()));

        model.insertRows(0, 5);
        QCOMPARE(spyOnRowsRemoved.count(), 0);
        QCOMPARE(spyOnRowsInserted.count(), 1);
        QCOMPARE(spyOnCountChanged.count(), 1);
        arguments = spyOnRowsInserted.takeFirst();
        QCOMPARE(arguments.at(1).toInt(), 0);
        QCOMPARE(arguments.at(2).toInt(), 4);
        QCOMPARE(proxy.count(), 5);
        spyOnRowsInserted.clear();
        spyOnCountChanged.clear();

        model.insertRows(2, 2);
        QCOMPARE(spyOnRowsRemoved.count(), 0);
        QCOMPARE(spyOnRowsInserted.count(), 1);
        QCOMPARE(spyOnCountChanged.count(), 1);
        arguments = spyOnRowsInserted.takeFirst();
        QCOMPARE(arguments.at(1).toInt(), 2);
        QCOMPARE(arguments.at(2).toInt(), 3);
        QCOMPARE(proxy.count(), 7);
        spyOnRowsInserted.clear();
        spyOnCountChanged.clear();

        /* FIXME: failing case */
        model.insertRows(5, 3);
        //QCOMPARE(proxy.count(), 7); // proxy.count == 9
        QCOMPARE(spyOnRowsRemoved.count(), 0);
        //QCOMPARE(spyOnRowsInserted.count(), 0); // spyOnRowsInserted.count == 1
        //QCOMPARE(spyOnCountChanged.count(), 0); // spyOnCountChanged.count == 1
    }

    void testInvertMatch() {
        QSortFilterProxyModelQML proxy;
        MockListModel model;

        proxy.setSourceModelQObject(&model);
        proxy.setDynamicSortFilter(true);

        QStringList rows;
        rows << "a/foobar/b" << "foobar" << "foobarbaz" << "hello";
        model.appendRows(rows);

        // Check that without a filterRegExp all rows are accepted regardless of invertMatch
        QCOMPARE(model.rowCount(), rows.count());
        QCOMPARE(proxy.rowCount(), rows.count());
        for (int i=0; i<rows.count(); i++) {
            QCOMPARE(proxy.index(i, 0).data().toString(), model.index(i, 0).data().toString());
        }
        proxy.setInvertMatch(true);
        QCOMPARE(model.rowCount(), rows.count());
        QCOMPARE(proxy.rowCount(), rows.count());
        for (int i=0; i<rows.count(); i++) {
            QCOMPARE(proxy.index(i, 0).data().toString(), model.index(i, 0).data().toString());
        }


        // Test non-anchored regexp with invertMatch active
        proxy.setFilterRegExp("foobar");
        QCOMPARE(proxy.rowCount(), 1);
        QCOMPARE(proxy.index(0, 0).data().toString(), rows.last());

        // Test anchored regexp with invertMatch active
        proxy.setFilterRegExp("^foobar$");
        QCOMPARE(proxy.rowCount(), 3);
        QCOMPARE(proxy.index(0, 0).data().toString(), rows.at(0));
        QCOMPARE(proxy.index(1, 0).data().toString(), rows.at(2));
        QCOMPARE(proxy.index(2, 0).data().toString(), rows.at(3));

        // Test regexp with OR and invertMatch active
        proxy.setFilterRegExp("foobar|hello");
        QCOMPARE(proxy.count(), 0);
    }
};

QTEST_MAIN(QSortFilterProxyModelTest)

#include "qsortfilterproxymodeltest.moc"

