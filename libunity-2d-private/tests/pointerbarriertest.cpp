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
#include <unitytestmacro.h>
#include <pointerbarrier.h>

// Qt
#include <QApplication>
#include <QSignalSpy>
#include <QX11Info>
#include <QtTestGui>

#include <X11/extensions/XTest.h>

class DisableTriggerZoneOnTriggerHelper : public QObject
{
    Q_OBJECT
public:
    DisableTriggerZoneOnTriggerHelper(PointerBarrierWrapper *barrier)
    {
        m_barrier = barrier;
        connect(barrier, SIGNAL(triggered()), this, SLOT(disable()));
    }

public Q_SLOTS:
    void disable()
    {
        m_barrier->setTriggerZoneEnabled(false);
    }

private:
    PointerBarrierWrapper *m_barrier;
};

class PointerBarrierTest : public QObject
{
    Q_OBJECT
private Q_SLOTS:
    void testBreak()
    {
        Display *display = QX11Info::display();
        PointerBarrierWrapper barrier;

        QSignalSpy brokenSpy(&barrier, SIGNAL(broken()));
        QSignalSpy triggeredSpy(&barrier, SIGNAL(triggered()));

        XTestFakeMotionEvent(display, -1, 50, 50, 0);
        QCOMPARE(QCursor::pos(), QPoint(50, 50));

        barrier.setP1(QPointF(100, 0));
        barrier.setP2(QPointF(100, 100));
        barrier.setThreshold(6500);
        barrier.setMaxVelocityMultiplier(2);
        barrier.setDecayRate(1500);
        barrier.setBreakPressure(2000);

        XTestFakeRelativeMotionEvent(display, 300, 0, 0);
        // We are stopped by the barrier and instead in 350, 50 we are in 99, 50
        QCOMPARE(QCursor::pos(), QPoint(99, 50));

        QCOMPARE(brokenSpy.count(), 0);
        QCOMPARE(triggeredSpy.count(), 0);

        for (int i = 0; i < 10; ++i) {
            XTestFakeRelativeMotionEvent(display, 100, 0, 0);
            QTest::qWait(100);
        }
        // We have broken the barrier and are somewhere else 
        QVERIFY(QCursor::pos() != QPoint(99, 50));
        QCOMPARE(brokenSpy.count(), 1);
        QCOMPARE(triggeredSpy.count(), 0);
    }

    void testTrigger()
    {
        Display *display = QX11Info::display();
        PointerBarrierWrapper barrier;

        QSignalSpy brokenSpy(&barrier, SIGNAL(broken()));
        QSignalSpy triggeredSpy(&barrier, SIGNAL(triggered()));

        XTestFakeMotionEvent(display, -1, 50, 50, 0);
        QCOMPARE(QCursor::pos(), QPoint(50, 50));

        barrier.setP1(QPointF(100, 0));
        barrier.setP2(QPointF(100, 100));
        barrier.setTriggerZoneP1(QPointF(100, 0));
        barrier.setTriggerZoneP2(QPointF(100, 100));
        barrier.setTriggerZoneEnabled(true);
        barrier.setThreshold(6500);
        barrier.setMaxVelocityMultiplier(2);
        barrier.setDecayRate(1500);
        barrier.setTriggerPressure(2000);
        barrier.setBreakPressure(2000);

        XTestFakeRelativeMotionEvent(display, 300, 0, 0);
        // We are stopped by the barrier and instead in 350, 50 we are in 99, 50
        QCOMPARE(QCursor::pos(), QPoint(99, 50));

        QCOMPARE(brokenSpy.count(), 0);
        QCOMPARE(triggeredSpy.count(), 0);

        for (int i = 0; i < 10; ++i) {
            XTestFakeRelativeMotionEvent(display, 100, 0, 0);
            QTest::qWait(100);
        }
        // We have triggered the barrier and are still there
        QCOMPARE(QCursor::pos(), QPoint(99, 50));
        QCOMPARE(brokenSpy.count(), 0);
        QVERIFY(triggeredSpy.count() >= 1);
    }

    void testTriggerAndBreak()
    {
        Display *display = QX11Info::display();
        PointerBarrierWrapper barrier;

        QSignalSpy brokenSpy(&barrier, SIGNAL(broken()));
        QSignalSpy triggeredSpy(&barrier, SIGNAL(triggered()));

        XTestFakeMotionEvent(display, -1, 50, 50, 0);
        QCOMPARE(QCursor::pos(), QPoint(50, 50));

        barrier.setP1(QPointF(100, 0));
        barrier.setP2(QPointF(100, 100));
        barrier.setTriggerZoneP1(QPointF(100, 0));
        barrier.setTriggerZoneP2(QPointF(100, 100));
        barrier.setTriggerZoneEnabled(true);
        barrier.setThreshold(6500);
        barrier.setMaxVelocityMultiplier(2);
        barrier.setDecayRate(1500);
        barrier.setTriggerPressure(2000);
        barrier.setBreakPressure(2000);

        DisableTriggerZoneOnTriggerHelper helper(&barrier);

        XTestFakeRelativeMotionEvent(display, 300, 0, 0);
        // We are stopped by the barrier and instead in 350, 50 we are in 99, 50
        QCOMPARE(QCursor::pos(), QPoint(99, 50));

        QCOMPARE(brokenSpy.count(), 0);
        QCOMPARE(triggeredSpy.count(), 0);

        for (int i = 0; i < 10; ++i) {
            XTestFakeRelativeMotionEvent(display, 100, 0, 0);
            QTest::qWait(100);
        }
        // We have triggered and broken the barrier and somewhere else
        QVERIFY(QCursor::pos() != QPoint(99, 50));
        QCOMPARE(brokenSpy.count(), 1);
        QCOMPARE(triggeredSpy.count(), 1);
    }

    void testTriggerWithoutAndBreak()
    {
        Display *display = QX11Info::display();
        PointerBarrierWrapper barrier;

        QSignalSpy brokenSpy(&barrier, SIGNAL(broken()));
        QSignalSpy triggeredSpy(&barrier, SIGNAL(triggered()));

        XTestFakeMotionEvent(display, -1, 50, 50, 0);
        QCOMPARE(QCursor::pos(), QPoint(50, 50));

        barrier.setP1(QPointF(100, 0));
        barrier.setP2(QPointF(100, 100));
        barrier.setTriggerZoneP1(QPointF(100, 0));
        barrier.setTriggerZoneP2(QPointF(100, 100));
        barrier.setTriggerZoneEnabled(true);
        barrier.setThreshold(6500);
        barrier.setMaxVelocityMultiplier(2);
        barrier.setDecayRate(1500);
        barrier.setTriggerPressure(2000);
        barrier.setBreakPressure(2000);

        DisableTriggerZoneOnTriggerHelper helper(&barrier);

        XTestFakeRelativeMotionEvent(display, 300, 0, 0);
        // We are stopped by the barrier and instead in 350, 50 we are in 99, 50
        QCOMPARE(QCursor::pos(), QPoint(99, 50));

        QCOMPARE(brokenSpy.count(), 0);
        QCOMPARE(triggeredSpy.count(), 0);

        for (int i = 0; i < 10 && barrier.triggerZoneEnabled(); ++i) {
            XTestFakeRelativeMotionEvent(display, 100, 0, 0);
            QTest::qWait(100);
        }
        // We have triggered the barrier
        QCOMPARE(QCursor::pos(), QPoint(99, 50));
        QCOMPARE(brokenSpy.count(), 0);
        QCOMPARE(triggeredSpy.count(), 1);

        // We can push a bit more without breaking the barrier
        for (int i = 0; i < 2; ++i) {
            XTestFakeRelativeMotionEvent(display, 100, 0, 0);
            QTest::qWait(100);
        }
        QCOMPARE(QCursor::pos(), QPoint(99, 50));
        QCOMPARE(brokenSpy.count(), 0);
        QCOMPARE(triggeredSpy.count(), 1);
    }

    void testTriggerAndBreakZones()
    {
        Display *display = QX11Info::display();
        PointerBarrierWrapper barrier;

        QSignalSpy brokenSpy(&barrier, SIGNAL(broken()));
        QSignalSpy triggeredSpy(&barrier, SIGNAL(triggered()));

        XTestFakeMotionEvent(display, -1, 50, 25, 0);
        QCOMPARE(QCursor::pos(), QPoint(50, 25));

        barrier.setP1(QPointF(100, 0));
        barrier.setP2(QPointF(100, 200));
        barrier.setTriggerZoneP1(QPointF(100, 50));
        barrier.setTriggerZoneP2(QPointF(100, 150));
        barrier.setTriggerZoneEnabled(true);
        barrier.setThreshold(6500);
        barrier.setMaxVelocityMultiplier(2);
        barrier.setDecayRate(1500);
        barrier.setTriggerPressure(2000);
        barrier.setBreakPressure(2000);

        DisableTriggerZoneOnTriggerHelper helper(&barrier);

        XTestFakeRelativeMotionEvent(display, 300, 0, 0);
        // We are stopped by the barrier and instead in 350, 25 we are in 99, 25
        QCOMPARE(QCursor::pos(), QPoint(99, 25));

        QCOMPARE(brokenSpy.count(), 0);
        QCOMPARE(triggeredSpy.count(), 0);

        for (int i = 0; i < 10; ++i) {
            XTestFakeRelativeMotionEvent(display, 100, 0, 0);
            QTest::qWait(100);
        }

        // We are above the trigger zone so we have only broke the barrier
        QVERIFY(QCursor::pos() != QPoint(99, 25));
        QCOMPARE(brokenSpy.count(), 1);
        QCOMPARE(triggeredSpy.count(), 0);

        // Go back
        XTestFakeMotionEvent(display, -1, 50, 100, 0);

        XTestFakeRelativeMotionEvent(display, 300, 0, 0);
        // We are stopped by the barrier and instead in 350, 100 we are in 99, 100
        QCOMPARE(QCursor::pos(), QPoint(99, 100));

        for (int i = 0; i < 10; ++i) {
            XTestFakeRelativeMotionEvent(display, 100, 0, 0);
            QTest::qWait(100);
        }

        // We are in the trigger zone so we have triggered and broken the barrier
        QVERIFY(QCursor::pos() != QPoint(99, 100));
        QCOMPARE(brokenSpy.count(), 2);
        QCOMPARE(triggeredSpy.count(), 1);
    }

    void testStopArea()
    {
        Display *display = QX11Info::display();
        PointerBarrierWrapper barrier;

        QSignalSpy brokenSpy(&barrier, SIGNAL(broken()));
        QSignalSpy triggeredSpy(&barrier, SIGNAL(triggered()));

        XTestFakeMotionEvent(display, -1, 50, 150, 0);
        QCOMPARE(QCursor::pos(), QPoint(50, 150));

        barrier.setP1(QPointF(100, 0));
        barrier.setP2(QPointF(100, 100));
        barrier.setThreshold(6500);
        barrier.setMaxVelocityMultiplier(2);
        barrier.setDecayRate(1500);
        barrier.setBreakPressure(2000);

        XTestFakeRelativeMotionEvent(display, 300, 0, 0);
        // We are not stopped by the barrier because it's above us
        // and are in 350, 150
        QCOMPARE(QCursor::pos(), QPoint(350, 150));

        QCOMPARE(brokenSpy.count(), 0);
        QCOMPARE(triggeredSpy.count(), 0);
    }
};

UAPP_TEST_MAIN(PointerBarrierTest)

#include "pointerbarriertest.moc"
