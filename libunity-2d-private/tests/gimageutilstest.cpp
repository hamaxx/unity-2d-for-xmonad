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

#include <gdk-pixbuf/gdk-pixbuf.h>

#include "gimageutils.h"
#include <config.h>

class GImageUtilsTest : public QObject
{
    Q_OBJECT
private Q_SLOTS:
    void test24bit()
    {
        GError *err = NULL;
        const QString path = unity2dDirectory() + "/libunity-2d-private/tests/verification/24bit.png";
        const GdkPixbuf *pixbuf = gdk_pixbuf_new_from_file(path.toLocal8Bit().constData(), &err);
        QVERIFY(!err);
        const QImage image = GImageUtils::imageForPixbuf(pixbuf, path);
        QCOMPARE(image.width(), 32);
        QCOMPARE(image.height(), 32);
        QVERIFY(!image.isNull());
        QCOMPARE(image.pixel(0, 0), qRgb(255, 0, 0));
        QCOMPARE(image.pixel(1, 0), qRgb(0, 255, 0));
        QCOMPARE(image.pixel(2, 0), qRgb(0, 0, 255));
    }

    void test24bit_2()
    {
        GError *err = NULL;
        const QString path = unity2dDirectory() + "/libunity-2d-private/tests/verification/24bit_2.png";
        const GdkPixbuf *pixbuf = gdk_pixbuf_new_from_file(path.toLocal8Bit().constData(), &err);
        QVERIFY(!err);
        const QImage image = GImageUtils::imageForPixbuf(pixbuf, path);
        QCOMPARE(image.width(), 100);
        QCOMPARE(image.height(), 100);
        QVERIFY(!image.isNull());
        QCOMPARE(image.pixel(3, 3), qRgb(250, 244, 216));
    }
};

QAPP_TEST_MAIN(GImageUtilsTest)

#include "gimageutilstest.moc"
