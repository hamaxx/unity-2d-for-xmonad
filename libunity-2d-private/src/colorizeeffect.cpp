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

#include "colorizeeffect.h"

#include <QPainter>

ColorizeEffect::ColorizeEffect(QObject *parent) :
    QGraphicsEffect(parent),
    m_color("black"),
    m_saturation(1.0)
{
}

QColor ColorizeEffect::color() const
{
    return m_color;
}

void ColorizeEffect::setColor(const QColor &color)
{
    if (color == m_color) return;

    m_color = color;
    m_tintedPixmap = QPixmap();
    update();
    Q_EMIT colorChanged(color);
}

qreal ColorizeEffect::saturation() const
{
    return m_saturation;
}

void ColorizeEffect::setSaturation(qreal saturation)
{
    if (saturation == m_saturation) return;

    m_saturation = saturation;
    m_tintedPixmap = QPixmap();
    update();
    Q_EMIT saturationChanged(saturation);
}

void ColorizeEffect::draw(QPainter *painter)
{
    QPoint offset;
    const QPixmap pixmap = sourcePixmap(Qt::DeviceCoordinates, &offset, QGraphicsEffect::NoPad);

    if (m_tintedPixmap.isNull()) {
        /* Compute the tinted pixmap by composing the source pixmap and a tinted rectangle */
        m_tintedPixmap = pixmap.copy();
        QPainter tintedPainter(&m_tintedPixmap);
        tintedPainter.setRenderHints(painter->renderHints());
        tintedPainter.setCompositionMode(QPainter::CompositionMode_Overlay);
        QColor color = m_color;
        color.setAlphaF(m_saturation);
        tintedPainter.fillRect(m_tintedPixmap.rect(), color);
        /* Apply alpha channel of the source pixmap to the tinted pixmap */
        tintedPainter.setCompositionMode(QPainter::CompositionMode_DestinationIn);
        tintedPainter.drawPixmap(0, 0, pixmap);
        tintedPainter.end();
    }

    QTransform previousTransform = painter->worldTransform();
    painter->setWorldTransform(QTransform());
    painter->drawPixmap(offset, m_tintedPixmap);
    painter->setWorldTransform(previousTransform);
}

void ColorizeEffect::sourceChanged(ChangeFlags flags)
{
    m_tintedPixmap = QPixmap();
}

#include "colorizeeffect.moc"
