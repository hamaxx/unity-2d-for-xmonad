/*
 * This file is part of unity-2d
 *
 * Copyright 2011 Canonical Ltd.
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
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

// Local
#include <unity2dtr.h>

// Qt
#include <QCoreApplication>
#include <QtTest>

#include <config-test.h>

class Unity2dTrTest : public QObject
{
    Q_OBJECT
private Q_SLOTS:
    void initTestCase()
    {
        Unity2dTr::init("unity2dtrtest", TEST_LOCALE_DIR);
    }

    void testTranslate()
    {
        QCOMPARE(u2dTr("Hello"), QString("Bonjour"));
        QCOMPARE(u2dTr("Not translated"), QString("Not translated"));
    }

    void testPluralHandling()
    {
        QCOMPARE(u2dTr("%n file", "%n files", 1), QString("1 fichier"));
        QCOMPARE(u2dTr("%n file", "%n files", 2), QString("2 fichiers"));
    }
};

QTEST_MAIN(Unity2dTrTest)

#include "unity2dtrtest.moc"
