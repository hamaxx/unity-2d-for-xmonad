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
// Self
#include "croppedlabel.h"

// Local

// Qt
#include <QImage>
#include <QPainter>

static const int FADEOUT_WIDTH = 16;

CroppedLabel::CroppedLabel(QWidget* parent)
: QLabel(parent)
{
}

QSize CroppedLabel::minimumSizeHint() const
{
    return QWidget::minimumSizeHint();
}

void CroppedLabel::paintEvent(QPaintEvent* event)
{
    QImage image(width(), height(), QImage::Format_ARGB32_Premultiplied);
    {
        QPainter painter(&image);
        painter.initFrom(this);
        painter.setCompositionMode(QPainter::CompositionMode_Source);
        painter.fillRect(rect(), Qt::transparent);

        painter.setCompositionMode(QPainter::CompositionMode_SourceOver);
        painter.drawText(contentsRect(), Qt::AlignLeft | Qt::AlignVCenter, text());

        if (QLabel::minimumSizeHint().width() > contentsRect().width()) {
            // Text does not fit, fade the end
            painter.setCompositionMode(QPainter::CompositionMode_DestinationIn);
            QRect gradientRect(width() - FADEOUT_WIDTH, 0, FADEOUT_WIDTH, height());
            QLinearGradient gradient(gradientRect.topLeft(), gradientRect.topRight());
            gradient.setColorAt(0, Qt::white);
            gradient.setColorAt(1, Qt::transparent);
            painter.fillRect(gradientRect, gradient);
        }
    }
    QPainter painter(this);
    painter.drawImage(0, 0, image);
}

#include "croppedlabel.moc"
