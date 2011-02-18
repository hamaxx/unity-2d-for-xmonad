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
#include <gettexttranslator.h>

// Qt
#include <QCoreApplication>
#include <QtTest>

#include <config-test.h>

class GettextTest : public QObject
{
    Q_OBJECT
private Q_SLOTS:
    void initTestCase()
    {
        GettextTranslator* translator = new GettextTranslator(QCoreApplication::instance());
        QVERIFY(translator->init("gettexttest", TEST_LOCALE_DIR));
        QCoreApplication::installTranslator(translator);
    }

    void testTranslate()
    {
        QCOMPARE(QCoreApplication::translate("", "Hello"),
            QString("Bonjour"));

        QCOMPARE(QCoreApplication::translate("Qt-context-is-ignored", "Hello"),
            QString("Bonjour"));

        QCOMPARE(QCoreApplication::translate("", "Not translated"),
            QString("Not translated"));
    }

    void testTranslateWithDisambiguation()
    {
        QCOMPARE(QCoreApplication::translate("", "Close", "Closing a door"),
            QString("Fermer"));

        QCOMPARE(QCoreApplication::translate("", "Close", "Not far"),
            QString::fromUtf8("Près"));
    }

    void testTranslateWithEmptyNonNullDisambiguation()
    {
        // qsTr in QML calls translate with an empty but non-null disambiguation string
        QCOMPARE(QCoreApplication::translate("", "Hello", ""),
            QString("Bonjour"));
    }
};

QTEST_MAIN(GettextTest)

#include "gettexttest.moc"
