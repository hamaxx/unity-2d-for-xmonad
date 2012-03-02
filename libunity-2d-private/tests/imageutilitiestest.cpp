/*
 * This file is part of unity-2d
 *
 * Copyright 2012 Canonical Ltd.
 *
 * Authors:
 * - Gerry Boland <gerry.boland@canonical.com>
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
#include <debug_p.h>
#include <imageutilities.h>
#include <config.h>

const int threshold = 0.001;

class ImageUtilitiesTest : public QObject
{
    Q_OBJECT
private Q_SLOTS:
    void initTestCase()
    {
    }

    void testAverageColorComputed0()
    {
        ImageUtilities imageUtil;
        QUrl image("file:" + unity2dDirectory() + "/libunity-2d-private/tests/verification/JardinPolar_by_CarmenGloria_Gonzalez.jpg");
        QColor color;

        imageUtil.setSource(image);
        color = imageUtil.averageColor();

        QCOMPARE(color.red(), 80);
        QCOMPARE(color.green(), 194);
        QCOMPARE(color.blue(), 230);
    }

    void testAverageColorComputed1()
    {
        ImageUtilities imageUtil;
        QUrl image("file:" + unity2dDirectory() + "/libunity-2d-private/tests/verification/Langelinie_Alle_by_SirPecanGum.jpg");
        QColor color;

        imageUtil.setSource(image);
        color = imageUtil.averageColor();
        QCOMPARE(color.red(), 230);
        QCOMPARE(color.green(), 126);
        QCOMPARE(color.blue(), 80);
    }

    void testAverageColorComputed2()
    {
        ImageUtilities imageUtil;
        QUrl image("file:" + unity2dDirectory() + "/libunity-2d-private/tests/verification/The_Grass_aint_Greener_by_fix_pena.jpg");
        QColor color;

        imageUtil.setSource(image);
        color = imageUtil.averageColor();

        QCOMPARE(color.red(), 218);
        QCOMPARE(color.green(), 230);
        QCOMPARE(color.blue(), 80);
    }

    void testAverageColorComputed3()
    {
        ImageUtilities imageUtil;
        QUrl image("file:" + unity2dDirectory() + "/libunity-2d-private/tests/verification/warty-final-ubuntu.png");
        QColor color;

        imageUtil.setSource(image);
        color = imageUtil.averageColor();

        QCOMPARE(color.red(), 230);
        QCOMPARE(color.green(), 80);
        QCOMPARE(color.blue(), 137);
    }
};

UAPP_TEST_MAIN(ImageUtilitiesTest)

#include "imageutilitiestest.moc"

