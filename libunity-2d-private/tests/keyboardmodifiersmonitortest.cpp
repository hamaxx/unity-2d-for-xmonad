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
#include <debug_p.h>
#include <keyboardmodifiersmonitor.h>
#include <unity2dapplication.h>

// Qt
#include <QSignalSpy>
#include <QtTestGui>
#include <QX11Info>

// X11
#include <X11/Xlib.h>
#include <X11/XKBlib.h>
#include <X11/extensions/XKB.h>

// Equivalent to QTEST_MAIN, but using Unity2dApplication instead of
// QApplication
#define UQ_TEST_MAIN(TestObject) \
int main(int argc, char *argv[]) \
{ \
    Unity2dApplication::earlySetup(argc, argv); \
    Unity2dApplication app(argc, argv); \
    QTEST_DISABLE_KEYPAD_NAVIGATION \
    TestObject tc; \
    return QTest::qExec(&tc, argc, argv); \
}

Q_DECLARE_METATYPE(Qt::KeyboardModifiers)

static void setModifierState(int modifier, bool on)
{
    XkbLatchModifiers(QX11Info::display(), XkbUseCoreKbd, modifier, on ? modifier : 0);
}

class KeyboardModifiersMonitorTest : public QObject
{
    Q_OBJECT
private Q_SLOTS:
    void initTestCase()
    {
        qRegisterMetaType<Qt::KeyboardModifiers>("Qt::KeyboardModifiers");
    }

    void testPressReleaseModifier_data()
    {
        QTest::addColumn<int>("x11Mask");
        QTest::addColumn<Qt::KeyboardModifiers>("qtModifiers");

        #define newRow(x11Mask, modifiers) \
            QTest::newRow(#modifiers) << (x11Mask) << Qt::KeyboardModifiers(modifiers)
        newRow(ShiftMask, Qt::ShiftModifier);
        newRow(ControlMask, Qt::ControlModifier);
        newRow(Mod1Mask, Qt::AltModifier);
        newRow(Mod4Mask, Qt::MetaModifier);
        #undef newRow
    }

    // Simulate pressing and releasing a modifier, checks the monitor notifies
    // us correctly
    void testPressReleaseModifier()
    {
        QFETCH(int, x11Mask);
        QFETCH(Qt::KeyboardModifiers, qtModifiers);
        KeyboardModifiersMonitor monitor;
        QSignalSpy spy(&monitor, SIGNAL(keyboardModifiersChanged(Qt::KeyboardModifiers)));

        setModifierState(x11Mask, true);
        QTest::qWait(200);
        QCOMPARE(monitor.keyboardModifiers(), qtModifiers);
        QCOMPARE(spy.count(), 1);
        QCOMPARE(spy.takeFirst().at(0).value<Qt::KeyboardModifiers>(), qtModifiers);

        setModifierState(x11Mask, false);
        QTest::qWait(200);
        QCOMPARE(monitor.keyboardModifiers(), 0);
        QCOMPARE(spy.count(), 1);
        QCOMPARE(spy.takeFirst().at(0).value<Qt::KeyboardModifiers>(), 0);
    }
};

UQ_TEST_MAIN(KeyboardModifiersMonitorTest)

#include "keyboardmodifiersmonitortest.moc"
