/*
 * Copyright (C) 2011 Canonical, Ltd.
 *
 * Authors:
 *  Ugo Riboni <ugo.riboni@canonical.com>
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

#include "iconutilities.h"

// libunity-2d
#include <debug_p.h>

// Qt
#include <QImage>
#include <QDeclarativeEngine>
#include <QDeclarativeContext>
#include <QDeclarativeImageProvider>

IconUtilities::IconUtilities(QDeclarativeEngine *engine) : QObject(engine), m_engine(engine)
{
}

/* Calculates both the background color and the glow color of a launcher tile
   based on the colors in the specified icon (using the same algorithm as Unity).
   The values are returned as list where the first item is the background color
   and the second one is the glow color.
*/
QList<QVariant>
IconUtilities::getColorsFromIcon(QUrl source, QSize size) const
{
    QList<QVariant> colors;

    // FIXME: we should find a way to avoid reloading the icon
    QImage icon = m_engine->imageProvider("icons")->requestImage(source.path().mid(1), &size, size);
    if (icon.width() == 0 || icon.height() == 0) {
        UQ_WARNING << "Unable to load icon in getColorsFromIcon from" << source;
        return colors;
    }

    long int rtotal = 0, gtotal = 0, btotal = 0;
    float total = 0.0f;

    for (int y = 0; y < icon.height(); ++y) {
        for (int x = 0; x < icon.width(); ++x) {
            QColor color = QColor::fromRgba(icon.pixel(x, y));

            float saturation = (qMax (color.red(), qMax (color.green(), color.blue())) -
                                qMin (color.red(), qMin (color.green(), color.blue()))) / 255.0f;
            float relevance = .1 + .9 * (color.alpha() / 255.0f) * saturation;

            rtotal += (unsigned char) (color.red() * relevance);
            gtotal += (unsigned char) (color.green() * relevance);
            btotal += (unsigned char) (color.blue() * relevance);

            total += relevance * 255;
        }
    }

    QColor hsv = QColor::fromRgbF(rtotal / total, gtotal / total, btotal / total).toHsv();

    /* Background color is the base color with 0.90f HSV value */
    hsv.setHsvF(hsv.hueF(),
                (hsv.saturationF() > .15f) ? 0.65f : hsv.saturationF(),
                0.90f);
    colors.append(QVariant::fromValue(hsv.toRgb()));

    /* Glow color is the base color with 1.0f HSV value */
    hsv.setHsvF(hsv.hueF(), hsv.saturationF(), 1.0f);
    colors.append(QVariant::fromValue(hsv.toRgb()));

    return colors;
}

#include "iconutilities.moc"
