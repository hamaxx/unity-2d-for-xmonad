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
};

QTEST_MAIN(ListAggregatorModelTest)

#include "listaggregatormodeltest.moc"

