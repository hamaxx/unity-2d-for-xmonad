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
    void testSetCurrentItemInvisible();
    void testSetCurrentIndexOnInvisibleItem();
    void testChangeCurrentIndex();
    void testNavigation();
    void testFlow();
    void testNavigationDirection();
    void testRTLNavigation();
    void testOrderChange();
    void testMakeFirstInvisible();
    void testMakeLastInvisibile();
    void testMakeLastVisible();
    void testMakeFirstVisible();
    void testSetPreviousItemInvisible();
    void testSetNextItemInvisible();

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

void FocusPathTest::testSetCurrentItemInvisible()
{
    /* Set an item invisible whose index is different from currentIndex, currentIndex should not change */
    QCOMPARE(m_focusPath->currentIndex(), 0);
    m_focusPath->path()[5].second->setVisible(false);
    QCOMPARE(m_focusPath->currentIndex(), 0);

    /* Set an item invisible whose index is currentIndex, currentIndex should change */
    m_focusPath->path()[0].second->setVisible(false);
    QCOMPARE(m_focusPath->currentIndex(), 1);
}

void FocusPathTest::testSetCurrentIndexOnInvisibleItem()
{
    QCOMPARE(m_focusPath->currentIndex(), 0);

    m_focusPath->setCurrentIndex(1);
    QCOMPARE(m_focusPath->currentIndex(), 1);

    m_focusPath->path()[5].second->setVisible(false);
    m_focusPath->setCurrentIndex(5);
    QCOMPARE(m_focusPath->currentIndex(), 1);
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

/*
 * Test RightToLeft navigation
 */
void FocusPathTest::testRTLNavigation()
{
    QGraphicsObject *root = m_view->rootObject();
    Q_ASSERT(root);
    QObject *grid = root->findChild<QObject*>("gridLayout");
    grid->setProperty("layoutDirection", 1);
    m_focusPath->setFlow(FocusPath::RightToLeft);


    QCOMPARE(m_focusPath->currentIndex(), 0);
    QTest::keyPress(m_view, Qt::Key_Down);
    QCOMPARE(m_focusPath->currentIndex(), 3);
    QTest::keyPress(m_view, Qt::Key_Down);
    QCOMPARE(m_focusPath->currentIndex(), 6);

    QTest::keyPress(m_view, Qt::Key_Left);
    QCOMPARE(m_focusPath->currentIndex(), 7);
    QTest::keyPress(m_view, Qt::Key_Left);
    QCOMPARE(m_focusPath->currentIndex(), 8);

    QTest::keyPress(m_view, Qt::Key_Up);
    QCOMPARE(m_focusPath->currentIndex(), 5);
    QTest::keyPress(m_view, Qt::Key_Up);
    QCOMPARE(m_focusPath->currentIndex(), 2);

    QTest::keyPress(m_view, Qt::Key_Right);
    QCOMPARE(m_focusPath->currentIndex(), 1);
    QTest::keyPress(m_view, Qt::Key_Right);
    QCOMPARE(m_focusPath->currentIndex(), 0);
}

/*
 * Test insert a new item in the midle of path
 */

void FocusPathTest::testOrderChange()
{
    /* Check if current index 3 is the item3 */
    QCOMPARE(m_focusPath->currentIndex(), 0);
    m_focusPath->setCurrentIndex(3);
    QCOMPARE(m_focusPath->currentItem()->property("itemData").toString(), QString("Item3"));


    QGraphicsObject *root = m_view->rootObject();
    Q_ASSERT(root);
    QObject *grid = root->findChild<QObject*>("gridLayout");

    /* Insert a new item with index 3 */
    QMetaObject::invokeMethod(grid, "insertItem", Q_ARG(QVariant, 3), Q_ARG(QVariant, "newItem3"));

    QCOMPARE(m_focusPath->path().size(), 10);

    /* Cehck if the item with index 3 is the new item */
    m_focusPath->setCurrentIndex(3);
    QCOMPARE(m_focusPath->currentItem()->property("itemData").toString(), QString("newItem3"));
}

/*
 * Test remove the first item from the path
 */
void FocusPathTest::testMakeFirstInvisible()
{
    QCOMPARE(m_focusPath->currentIndex(), 0);
    m_focusPath->path()[0].second->setVisible(false);
    QCOMPARE(m_focusPath->currentIndex(), 1);
}

/*
 * Test remove the last item from the path
 */
void FocusPathTest::testMakeLastInvisibile()
{
    int lastIndex = m_focusPath->path().size() - 1;
    m_focusPath->setCurrentIndex(lastIndex);
    m_focusPath->path()[lastIndex].second->setVisible(false);
    QCOMPARE(m_focusPath->currentIndex(), lastIndex-1);
}


/*
 * Test remove any previous item from the path
 */
void FocusPathTest::testSetPreviousItemInvisible()
{
    int oldSize = m_focusPath->path().size();
    m_focusPath->setCurrentIndex(5);
    m_focusPath->path()[1].second->setVisible(false);
    QCOMPARE(m_focusPath->currentIndex(), 5);
    QCOMPARE(m_focusPath->path().size(), oldSize - 1);
}

/*
 * Test remove any next item from the path
 */
void FocusPathTest::testSetNextItemInvisible()
{
    int oldSize = m_focusPath->path().size();
    m_focusPath->setCurrentIndex(5);
    m_focusPath->path()[7].second->setVisible(false);
    QCOMPARE(m_focusPath->currentIndex(), 5);
    QCOMPARE(m_focusPath->path().size(), oldSize - 1);
}

/*
 * Teste insert a item in the beginer
 */
void FocusPathTest::testMakeFirstVisible()
{
    QDeclarativeItem *firstItem = m_focusPath->path()[0].second;
    QCOMPARE(m_focusPath->currentIndex(), 0);
    firstItem->setVisible(false);
    QCOMPARE(m_focusPath->currentIndex(), 1);
    m_focusPath->setCurrentIndex(0);
    QCOMPARE(m_focusPath->currentIndex(), 1);
    firstItem->setVisible(true);
    QCOMPARE(m_focusPath->currentIndex(), 1);
    m_focusPath->setCurrentIndex(0);
    QCOMPARE(m_focusPath->currentIndex(), 0);
}

/*
 * Test insert a item in the end of the path
 */
void FocusPathTest::testMakeLastVisible()
{
    int lastIndex = m_focusPath->path().size() - 1;
    QDeclarativeItem *lastItem = m_focusPath->path()[lastIndex].second;

    m_focusPath->setCurrentIndex(lastIndex);
    lastItem->setVisible(false);
    QCOMPARE(m_focusPath->currentIndex(), lastIndex - 1);
    m_focusPath->setCurrentIndex(lastIndex);
    QCOMPARE(m_focusPath->currentIndex(), lastIndex - 1);
    lastItem->setVisible(true);
    m_focusPath->setCurrentIndex(lastIndex);
    QCOMPARE(m_focusPath->currentIndex(), lastIndex);
}


UAPP_TEST_MAIN(FocusPathTest)

#include "focuspathtest.moc"
