/*
 * Copyright (C) 2011 Canonical, Ltd.
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

// local
#include <listaggregatormodel.h>

// Qt
#include <QTest>
#include <QStringListModel>

class ListAggregatorModelTest : public QObject
{
    Q_OBJECT

private Q_SLOTS:

    void testRoleNames()
    {
        ListAggregatorModel model;
        const QHash<int, QByteArray> roleNames = model.roleNames();
        QCOMPARE(roleNames.size(), 1);
        QVERIFY(roleNames.contains(0));
        QCOMPARE(roleNames[0], QByteArray("item"));
    }

    void testAppendModelWrongType()
    {
        ListAggregatorModel model;
        QVERIFY(model.m_models.isEmpty());
        QTest::ignoreMessage(QtWarningMsg, "void ListAggregatorModel::appendModel(const QVariant&): Unable to append a model that is not of type QAbstractListModel. Invalid model. ");
        model.appendModel(QVariant());
        QVERIFY(model.m_models.isEmpty());
        QTest::ignoreMessage(QtWarningMsg, "void ListAggregatorModel::appendModel(const QVariant&): Unable to append a model that is not of type QAbstractListModel. QVariant(bool, true) is of type bool ");
        model.appendModel(QVariant(true));
        QVERIFY(model.m_models.isEmpty());
        QTest::ignoreMessage(QtWarningMsg, "void ListAggregatorModel::appendModel(const QVariant&): Unable to append a model that is not of type QAbstractListModel. \"\" is of type QObject ");
        model.appendModel(QVariant::fromValue(new QObject));
        QVERIFY(model.m_models.isEmpty());
    }

    void testAggregateListModel()
    {
        ListAggregatorModel model;
        QVERIFY(model.m_models.isEmpty());
        QCOMPARE(model.rowCount(), 0);

        QStringListModel list1(QStringList() << "aa" << "ab" << "ac");
        model.aggregateListModel(&list1);
        QCOMPARE(model.m_models.size(), 1);
        QCOMPARE(qobject_cast<QStringListModel*>(model.m_models[0]), &list1);
        QCOMPARE(model.rowCount(), 3);

        QStringListModel list2(QStringList() << "ba" << "bb" << "bc" << "bd");
        model.aggregateListModel(&list2);
        QCOMPARE(model.m_models.size(), 2);
        QCOMPARE(qobject_cast<QStringListModel*>(model.m_models[1]), &list2);
        QCOMPARE(model.rowCount(), 7);
    }

    void testRemoveListModel()
    {
        ListAggregatorModel model;
        QStringListModel list1(QStringList() << "aa" << "ab" << "ac");
        model.aggregateListModel(&list1);
        QStringListModel list2(QStringList() << "ba" << "bb" << "bc" << "bd");
        model.aggregateListModel(&list2);
        QStringListModel list3(QStringList() << "ca" << "cb");
        model.aggregateListModel(&list3);

        model.removeListModel(&list2);
        QCOMPARE(model.m_models.size(), 2);
        QCOMPARE(qobject_cast<QStringListModel*>(model.m_models[0]), &list1);
        QCOMPARE(qobject_cast<QStringListModel*>(model.m_models[1]), &list3);
        QCOMPARE(model.rowCount(), 5);
    }

    void testData()
    {
        ListAggregatorModel model;
        model.aggregateListModel(new QStringListModel(QStringList() << "aa" << "ab" << "ac", &model));
        model.aggregateListModel(new QStringListModel(QStringList() << "ba" << "bb" << "bc" << "bd", &model));
        model.aggregateListModel(new QStringListModel(QStringList() << "ca" << "cb", &model));
        QStringList data = QStringList() << "aa" << "ab" << "ac" << "ba" << "bb" << "bc" << "bd" << "ca" << "cb";
        for (int i = 0; i < model.rowCount(); ++i) {
            QCOMPARE(model.data(model.index(i)).toString(), data[i]);
        }
    }

    void testGet()
    {
        ListAggregatorModel model;
        model.aggregateListModel(new QStringListModel(QStringList() << "aa" << "ab" << "ac", &model));
        model.aggregateListModel(new QStringListModel(QStringList() << "ba" << "bb" << "bc" << "bd", &model));
        model.aggregateListModel(new QStringListModel(QStringList() << "ca" << "cb", &model));
        QStringList data = QStringList() << "aa" << "ab" << "ac" << "ba" << "bb" << "bc" << "bd" << "ca" << "cb";
        for (int i = 0; i < model.rowCount(); ++i) {
            QCOMPARE(model.get(i).toString(), data[i]);
        }
    }
};

QTEST_MAIN(ListAggregatorModelTest)

#include "listaggregatormodeltest.moc"

