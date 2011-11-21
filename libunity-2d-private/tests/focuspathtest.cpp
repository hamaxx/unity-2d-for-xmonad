/*
 * This file is part of unity-2d
 *
 * Copyright 2010 Canonical Ltd.
 *
 * Authors:
 * - Renato Araujo Oliveira Filho <renato.filho@canonical.com>
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

#include <unitytestmacro.h>
#include <config.h>
#include <focuspath.h>

#include <QGraphicsObject>
#include <QDeclarativeEngine>
#include <QObject>
#include <QDeclarativeView>
#include <QtTestGui>

class FocusPathTest : public QObject
{
    Q_OBJECT
private Q_SLOTS:
    void init();
    void cleanup();
    void testInitialization();
    void testChangeCurrentIndex();
    void testNavigation();
    void testFlow();
    void testNavigationDirection();

private:
    QDeclarativeView *m_view;
    FocusPath *m_focusPath;

    void createView();
};


/*
 * Loads a QML file with a grix 3x3 elements
 */

void FocusPathTest::init()
{
    m_view = new QDeclarativeView;
    m_view->engine()->addImportPath(unity2dImportPath());
    m_view->setSource(QUrl(QString("%1/libunity-2d-private/tests/focuspathtest.qml").arg(unity2dDirectory())));

    QGraphicsObject *root = m_view->rootObject();
    Q_ASSERT(root);
    m_focusPath = root->findChild<FocusPath*>("focusPath");
    Q_ASSERT(m_focusPath);
 
}

void FocusPathTest::cleanup()
{
    m_focusPath = 0;
    delete m_view;
    m_view = 0;
}

/*
 * Check if all items was inserted in FocusPath
 */
void FocusPathTest::testInitialization()
{
    QList<PathItem > path = m_focusPath->path();
    int focusPathSize = path.size();
    QCOMPARE(focusPathSize, 9);
    for(int i=0; i < focusPathSize; i++) {
        QString itemName = QString("Item%1").arg(i);
        QCOMPARE(path[i].second->property("itemData").toString(), itemName);
    }

    /* Tun a item ivisible, it must be removed from the path*/
    path[5].second->setVisible(false);
    QCOMPARE(m_focusPath->path().size(), 8);
}

/*
 * Test change currentIndex manually
 */
void FocusPathTest::testChangeCurrentIndex()
{
    QCOMPARE(m_focusPath->currentIndex(), 0);
    m_focusPath->setCurrentIndex(5);
    QCOMPARE(m_focusPath->currentIndex(), 5);
    QCOMPARE(m_focusPath->currentItem()->property("itemData").toString(), QString("Item5"));

    /* Set a invalid index */
    m_focusPath->setCurrentIndex(10);

    /* It must keep the previous index */
    QCOMPARE(m_focusPath->currentIndex(), 5);
    QCOMPARE(m_focusPath->currentItem()->property("itemData").toString(), QString("Item5"));
}

/*
 * Test if keyboard navigation is working correct
 */
void FocusPathTest::testNavigation()
{
    QCOMPARE(m_focusPath->currentItem()->property("itemData").toString(), QString("Item0"));
    /* Horizontal navigation */
    int index = 0;
    do {
        QTest::keyPress(m_view, Qt::Key_Right);
        QCOMPARE(m_focusPath->currentItem()->property("itemData").toString(), QString("Item%1").arg(++index));
        QCOMPARE(m_focusPath->currentIndex(), index);
    } while(m_focusPath->currentIndex() < 8);

    do {
        QTest::keyPress(m_view, Qt::Key_Left);
        QCOMPARE(m_focusPath->currentItem()->property("itemData").toString(), QString("Item%1").arg(--index));
        QCOMPARE(m_focusPath->currentIndex(), index);
    } while (m_focusPath->currentIndex() > 0);

    /* Vertical navigation */
    QTest::keyPress(m_view, Qt::Key_Down);
    QCOMPARE(m_focusPath->currentItem()->property("itemData").toString(), QString("Item3"));
    QTest::keyPress(m_view, Qt::Key_Down);
    QCOMPARE(m_focusPath->currentItem()->property("itemData").toString(), QString("Item6"));
    QTest::keyPress(m_view, Qt::Key_Right);
    QCOMPARE(m_focusPath->currentItem()->property("itemData").toString(), QString("Item7"));
    QTest::keyPress(m_view, Qt::Key_Up);
    QCOMPARE(m_focusPath->currentItem()->property("itemData").toString(), QString("Item4"));
    QTest::keyPress(m_view, Qt::Key_Up);
    QCOMPARE(m_focusPath->currentItem()->property("itemData").toString(), QString("Item1"));
}

/*
 * Test Flow property with value TopToBottom 
 */
void FocusPathTest::testFlow()
{
    QGraphicsObject *root = m_view->rootObject();
    Q_ASSERT(root);
    QObject *grid = root->findChild<QObject*>("gridLayout");
    grid->setProperty("flow", 1);
    QCOMPARE(m_focusPath->flow(), FocusPath::TopToBottom);

    QCOMPARE(m_focusPath->currentIndex(), 0);
    QTest::keyPress(m_view, Qt::Key_Down);
    QCOMPARE(m_focusPath->currentIndex(), 1);
    QTest::keyPress(m_view, Qt::Key_Down);
    QCOMPARE(m_focusPath->currentIndex(), 2);
    QTest::keyPress(m_view, Qt::Key_Right);
    QCOMPARE(m_focusPath->currentIndex(), 5);
    QTest::keyPress(m_view, Qt::Key_Right);
    QCOMPARE(m_focusPath->currentIndex(), 8);
    QTest::keyPress(m_view, Qt::Key_Up);
    QCOMPARE(m_focusPath->currentIndex(), 7);
    QTest::keyPress(m_view, Qt::Key_Up);
    QCOMPARE(m_focusPath->currentIndex(), 6);
    QTest::keyPress(m_view, Qt::Key_Left);
    QCOMPARE(m_focusPath->currentIndex(), 3);
    QTest::keyPress(m_view, Qt::Key_Left);
    QCOMPARE(m_focusPath->currentIndex(), 0);
}

/*
 * Test NavigationDirection limitations
 */
void FocusPathTest::testNavigationDirection()
{
    m_focusPath->setDirection(FocusPath::Horizontal);
    QTest::keyPress(m_view, Qt::Key_Down);
    QCOMPARE(m_focusPath->currentIndex(), 0);
    QTest::keyPress(m_view, Qt::Key_Right);
    QCOMPARE(m_focusPath->currentIndex(), 1);
    QTest::keyPress(m_view, Qt::Key_Left);
    QCOMPARE(m_focusPath->currentIndex(), 0);

    m_focusPath->setDirection(FocusPath::Vertical);
    QTest::keyPress(m_view, Qt::Key_Right);
    QCOMPARE(m_focusPath->currentIndex(), 0);
    QTest::keyPress(m_view, Qt::Key_Down);
    QCOMPARE(m_focusPath->currentIndex(), 3);
    QTest::keyPress(m_view, Qt::Key_Up);
    QCOMPARE(m_focusPath->currentIndex(), 0);
}


UAPP_TEST_MAIN(FocusPathTest)

#include "focuspathtest.moc"
