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
#ifndef FAKECAIRO_H
#define FAKECAIRO_H

// NuxCore
#include <NuxCore/Color.h>

// Qt
#include <QPainter>
#include <QPainterPath>

/*
 * This module attempts to fake Cairo calls using QPainter, making it easier to
 * port Cairo paint operations to Qt
 */
struct fcairo_t
{
    fcairo_t(QPainter* _painter)
    : painter(_painter)
    {
        painter->save();
    }

    ~fcairo_t()
    {
        painter->restore();
    }

    QPainter* painter;
    QPainterPath path;
};

inline void fcairo_arc(fcairo_t& cr, qreal xc, qreal yc, qreal radius, qreal angle1, qreal angle2)
{
    QRectF rect(xc - radius, yc - radius, radius * 2, radius * 2);

    while (angle2 < angle1) {
        angle2 += 2 * M_PI;
    }

    qreal start = (2. - angle1 / M_PI) * 180;
    qreal stop  = (2. - angle2 / M_PI) * 180;
    cr.path.arcTo(rect, start, stop - start);
}

inline void fcairo_move_to(fcairo_t& cr, qreal x, qreal y)
{
    cr.path.moveTo(x, y);
}

inline void fcairo_line_to(fcairo_t& cr, qreal x, qreal y)
{
    cr.path.lineTo(x, y);
}

inline void fcairo_fill_preserve(fcairo_t& cr)
{
    cr.painter->fillPath(cr.path, cr.painter->brush());
}

inline void fcairo_stroke(fcairo_t& cr)
{
    QPen pen(cr.painter->brush().color(), 1);
    cr.painter->strokePath(cr.path, pen);
    cr.path = QPainterPath();
}

typedef QGradient fcairo_pattern_t;

inline fcairo_pattern_t* fcairo_pattern_create_linear (qreal x1, qreal y1, qreal x2, qreal y2)
{
    return new QLinearGradient(x1, y1, x2, y2);
}

inline void fcairo_pattern_destroy(fcairo_pattern_t* pattern)
{
    delete pattern;
}

inline void fcairo_pattern_add_color_stop_rgba(fcairo_pattern_t* pattern, qreal offset, qreal r, qreal g, qreal b, qreal a)
{
    pattern->setColorAt(offset, QColor::fromRgbF(r, g, b, a));
}

inline void fcairo_set_source(fcairo_t& cr, fcairo_pattern_t* pattern)
{
    cr.painter->setPen(Qt::NoPen);
    cr.painter->setBrush(*pattern);
}

inline void fcairo_set_source_rgb(fcairo_t& cr, qreal r, qreal g, qreal b)
{
    cr.painter->setBrush(QColor::fromRgbF(r, g, b));
}

inline nux::color::Color nuxColorFromQColor(const QColor& qColor)
{
    nux::color::Color color;
    color.red = qColor.redF();
    color.green = qColor.greenF();
    color.blue = qColor.blueF();
    color.alpha = qColor.alphaF();
    return color;
}

#endif /* FAKECAIRO_H */
