/*
 * This file is part of unity-2d
 *
 * Copyright 2011 Canonical Ltd.
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
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */


#ifndef UNITYTESTMACRO_H
#define UNITYTESTMACRO_H

#include <QtTestGui>
#include <QWindowsStyle>
#include <unity2dapplication.h>

/*
 * This macro is equivalent to QTEST_MAIN, but we use a windows style to avoid problems with Gtk version.
 */
#define QAPP_TEST_MAIN(TestObject) \
int main(int argc, char *argv[]) \
{ \
        QApplication::setStyle(new QWindowsStyle);\
        QApplication app(argc, argv); \
        QTEST_DISABLE_KEYPAD_NAVIGATION \
        TestObject tc; \
        return QTest::qExec(&tc, argc, argv); \
}

/*
 * Equivalent to QTEST_MAIN, but using Unity2dApplication instead of QApplication
 */
#define UAPP_TEST_MAIN(TestObject) \
int main(int argc, char *argv[]) \
{ \
    Unity2dApplication::earlySetup(argc, argv); \
    Unity2dApplication app(argc, argv); \
    QTEST_DISABLE_KEYPAD_NAVIGATION \
    TestObject tc; \
    return QTest::qExec(&tc, argc, argv); \
}

#endif
