/*
 * Copyright (C) 2010 Canonical, Ltd.
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

#include "blendedimageprovider.h"
#include <QPainter>

BlendedImageProvider::BlendedImageProvider() : QDeclarativeImageProvider(QDeclarativeImageProvider::Image)
{
}

BlendedImageProvider::~BlendedImageProvider()
{
}

QImage BlendedImageProvider::requestImage(const QString &id, QSize *size, const QSize &requestedSize)
{
    /* id is of the form [FILENAME]color=[COLORNAME]alpha=[FLOAT] */
    QRegExp rx("(.+)color=(.+)alpha=(\\d+(?:\.\\d+)?)");
    rx.indexIn(id);
    QStringList list = rx.capturedTexts();
    if(list.size() != 4)
        return QImage();

    QString filename = list[1];
    QColor color;
    color.setNamedColor(list[2]);
    float alpha = list[3].toFloat();
    color.setAlphaF(alpha);

    QImage image(filename);
    if(requestedSize.width() == 0 && requestedSize.height() != 0)
        image = image.scaledToHeight(requestedSize.height(), Qt::SmoothTransformation);
    else if(requestedSize.width() != 0 && requestedSize.height() == 0)
        image = image.scaledToWidth(requestedSize.width(), Qt::SmoothTransformation);
    else if(requestedSize.isValid())
        image = image.scaled(requestedSize, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    else
        return QImage();

    if(size)
        *size = image.size();

    QPainter painter(&image);
    painter.setCompositionMode(QPainter::CompositionMode_SourceOver);
    painter.fillRect(image.rect(), color);
    painter.end();

    return image;
}
