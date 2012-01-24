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
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

// Local
#include <unity2dpanel.h>

// Qt
#include <QApplication>
#include <QDesktopWidget>
#include <QtTestGui>

class PanelTest : public QObject
{
    Q_OBJECT
private Q_SLOTS:
    void testSetEdge()
    {
        QRect screen = QApplication::desktop()->screenGeometry();
        QRect available = QApplication::desktop()->availableGeometry();

        {
            Unity2dPanel panel;
            panel.setFixedHeight(16);
            panel.setEdge(Unity2dPanel::TopEdge);
            panel.show();
            QTest::qWait(500);

            QRect expectedGeometry = QRect(screen.left(), screen.top(), screen.width(), 16);
            QCOMPARE(panel.geometry(), expectedGeometry);
            QCOMPARE(QApplication::desktop()->availableGeometry(), available.adjusted(0, 16, 0, 0));
        }

        // Strut should be released when panel is deleted
        QTest::qWait(500);
        QCOMPARE(QApplication::desktop()->availableGeometry(), available);
    }

    void testSetUseStrut()
    {
        QRect screen = QApplication::desktop()->screenGeometry();
        QRect available = QApplication::desktop()->availableGeometry();

        Unity2dPanel panel;
        panel.setFixedHeight(16);
        panel.setUseStrut(false);
        panel.setEdge(Unity2dPanel::TopEdge);
        panel.show();
        QTest::qWait(500);

        QCOMPARE(QApplication::desktop()->availableGeometry(), available);

        panel.setUseStrut(true);
        QTest::qWait(500);

        QCOMPARE(QApplication::desktop()->availableGeometry(), available.adjusted(0, 16, 0, 0));

        panel.setUseStrut(false);
        QTest::qWait(500);
        QCOMPARE(QApplication::desktop()->availableGeometry(), available);
    }

    void testAddQWidget()
    {
        Unity2dPanel panel;
        QWidget* widget1 = new QWidget;
        QWidget* widget2 = new QWidget;
        panel.addWidget(widget1);
        panel.addWidget(widget2);
        panel.show();

        QCOMPARE(widget1->geometry(), QRect(0, 0, panel.width() / 2, panel.height()));
        QCOMPARE(widget2->geometry(), QRect(panel.width() / 2, 0, panel.width() / 2, panel.height()));
    }

    void testAddSpacer()
    {
        Unity2dPanel panel;
        QWidget* widget1 = new QWidget;
        widget1->setFixedWidth(100);
        QWidget* widget2 = new QWidget;
        widget2->setFixedWidth(200);
        panel.addWidget(widget1);
        panel.addSpacer();
        panel.addWidget(widget2);
        panel.show();

        QCOMPARE(widget1->geometry(), QRect(0, 0, 100, panel.height()));
        QCOMPARE(widget2->geometry(), QRect(panel.width() - 200, 0, 200, panel.height()));
    }
};

QTEST_MAIN(PanelTest)

#include "paneltest.moc"
