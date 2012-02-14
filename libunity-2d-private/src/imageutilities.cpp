/*
 * Copyright (C) 2012 Canonical, Ltd.
 *
 * Authors:
 *  Florian Boucault <florian.boucault@canonical.com>
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

#include "imageutilities.h"

#include <QImage>

ImageUtilities::ImageUtilities(QObject *parent) :
    QObject(parent)
{
}

QUrl ImageUtilities::source() const
{
    return m_source;
}

QColor ImageUtilities::averageColor() const
{
    return m_averageColor;
}

void ImageUtilities::setSource(const QUrl &source)
{
    /* FIXME: should monitor source file for changes */
    if (source == m_source) return;
    m_source = source;
    Q_EMIT sourceChanged();

    QImage image;
    image.load(source.toLocalFile());

    if (image.isNull()) return;

    QColor averageColor = computeAverageColor(image);

    if (averageColor != m_averageColor) {
        m_averageColor = averageColor;
        Q_EMIT averageColorChanged();
    }
}

QColor ImageUtilities::computeAverageColor(const QImage& image)
{
    long int rtotal = 0, gtotal = 0, btotal = 0;
    float total = 0.0f;
    // always sample 100x100 pixels
    int samplePoints = 100;
    int stepX = std::max(image.width() / samplePoints, 1);
    int stepY = std::max(image.height() / samplePoints, 1);

    for (int y = 0; y < image.height(); y += stepY) {
        for (int x = 0; x < image.width(); x += stepX) {
            QColor color = QColor::fromRgba(image.pixel(x, y));

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
    return hsv;
}

#include "imageutilities.moc"
