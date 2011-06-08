/*
 * This file is part of unity-2d
 *
 * Copyright 2010 Canonical Ltd.
 *
 * Authors:
 * - Aurélien Gâteau <aurelien.gateau@canonical.com>
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
 * along with this program.  If not, see <http://www.gnu.org/licenses/>
 */

// Local
#include <launchermenu.h>

// Qt
#include <QtTestGui>

class LauncherMenuTest : public QObject
{
    Q_OBJECT
private Q_SLOTS:
    void testSetTitle_data()
    {
        QTest::addColumn<QString>("title");
        QTest::addColumn<QString>("expected");

        QTest::newRow("Foo App") << "Foo App" << "Foo App";
        QTest::newRow("Rock&Roll") << "Rock&Roll" << "Rock&&Roll";
    }

    void testSetTitle()
    {
        QFETCH(QString, title);
        QFETCH(QString, expected);
        LauncherContextualMenu menu;
        QVERIFY(menu.title().isEmpty());

        menu.setTitle(title);
        QCOMPARE(menu.actions().count(), 1);
        QCOMPARE(menu.actions().first()->text(), expected);
    }
};

QTEST_MAIN(LauncherMenuTest)

#include "launchermenutest.moc"

