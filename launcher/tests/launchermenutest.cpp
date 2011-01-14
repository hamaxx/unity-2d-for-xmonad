/*
 * This file is part of unity-2d
 *
 * Copyright 2010 Canonical Ltd.
 *
 * Authors:
 * - Aurélien Gâteau <aurelien.gateau@canonical.com>
 *
 * License: GPL v3
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

