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

#ifndef COLORIZEEFFECT_H
#define COLORIZEEFFECT_H

#include <QGraphicsEffect>
#include <QColor>

class ColorizeEffect : public QGraphicsEffect
{
    Q_OBJECT

    Q_PROPERTY(QColor color READ color WRITE setColor NOTIFY colorChanged)
    Q_PROPERTY(qreal saturation READ saturation WRITE setSaturation NOTIFY saturationChanged)

public:
    explicit ColorizeEffect(QObject *parent = 0);

    // getters
    QColor color() const;
    qreal saturation() const;

    // setters
    void setColor(const QColor &color);
    void setSaturation(qreal saturation);

Q_SIGNALS:
    void colorChanged(const QColor &color);
    void saturationChanged(qreal saturation);

protected:
    void draw(QPainter *painter);
    void sourceChanged(ChangeFlags flags);

private:
    QColor m_color;
    qreal m_saturation;

    // caching of intermediary renderings
    QPixmap m_tintedPixmap;
};

#endif // COLORIZEEFFECT_H
