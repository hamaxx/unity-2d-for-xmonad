/*
 * This file is part of unity-2d
 *
 * Copyright 2012 Canonical Ltd.
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

#include <QObject>
#include <unitytestmacro.h>

#include "gkeysequenceparser.h"

class GKeySequenceParserTest : public QObject
{
    Q_OBJECT
private Q_SLOTS:
    void testParsing()
    {
        int x11Code;
        Qt::KeyboardModifiers modifiers;
        bool success;

        success = GKeySequenceParser::parse("<Alt>", &x11Code, &modifiers);
        QVERIFY(success);
        QCOMPARE(x11Code, 0);
        QCOMPARE(modifiers, Qt::AltModifier);

        success = GKeySequenceParser::parse("<Control><Shift><Alt><Super>ccedilla", &x11Code, &modifiers);
        QVERIFY(success);
        QCOMPARE(x11Code, 51);
        QCOMPARE(modifiers, Qt::AltModifier | Qt::ShiftModifier | Qt::ControlModifier | Qt::MetaModifier);

        success = GKeySequenceParser::parse("<Control><Shift><Alt><Super>ccedillaj", &x11Code, &modifiers);
        QVERIFY(!success);

        success = GKeySequenceParser::parse("<Controll><Shift><Alt><Super>ccedillaj", &x11Code, &modifiers);
        QVERIFY(!success);

        success = GKeySequenceParser::parse("<Control<Shift><Alt><Super>ccedillaj", &x11Code, &modifiers);
        QVERIFY(!success);

        success = GKeySequenceParser::parse("<Alt>+p", &x11Code, &modifiers);
        QVERIFY(!success);
    }
};

QAPP_TEST_MAIN(GKeySequenceParserTest)

#include "gkeysequenceparser.moc"