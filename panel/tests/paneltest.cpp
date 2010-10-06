/*
 * This file is part of unity-qt
 *
 * Copyright 2010 Canonical Ltd.
 *
 * Authors:
 * - Aurélien Gâteau <aurelien.gateau@canonical.com>
 *
 * License: GPL v3
 */

// Local
#include <panel.h>

// Qt
#include <QApplication>
#include <QDesktopWidget>
#include <QtTestGui>

using namespace UnityQt;

class PanelTest : public QObject
{
    Q_OBJECT
private Q_SLOTS:
    void testSetEdge()
    {
        QRect screen = QApplication::desktop()->screenGeometry();
        QRect available = QApplication::desktop()->availableGeometry();

        Panel panel;
        panel.setFixedHeight(16);
        panel.setEdge(Panel::TopEdge);
        panel.show();
        QTest::qWait(500);

        QRect expectedGeometry = QRect(screen.left(), screen.top(), screen.width(), 16);
        QCOMPARE(panel.geometry(), expectedGeometry);
        QCOMPARE(QApplication::desktop()->availableGeometry(), available.adjusted(0, 16, 0, 0));
    }

    void testAddQWidget()
    {
        Panel panel;
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
        Panel panel;
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
