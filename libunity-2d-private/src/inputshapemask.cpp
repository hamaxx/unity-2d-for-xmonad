/*
 * This file is part of unity-2d
 *
 * Copyright 2012 Canonical Ltd.
 *
 * Authors:
 * - Ugo Riboni <ugo.riboni@canonical.com>
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

#include "inputshapemask.h"
#include "config.h"

#include <QBitmap>
#include <QDebug>

InputShapeMask::InputShapeMask(QObject *parent) :
    QObject(parent),
    m_enabled(true)
{
}

void InputShapeMask::updateShape()
{
    QBitmap newShape;

    if (!m_source.isEmpty() && m_color.isValid()) {
        QPixmap image;
        QString path = unity2dDirectory() + "/" + m_source;
        if (image.load(path)) {
            newShape = image.createMaskFromColor(m_color.rgb(), Qt::MaskInColor);
        } else {
            qWarning() << "Failed to load input shape mask image from" << path;
        }
    }

    m_shape = newShape;
    Q_EMIT shapeChanged();
}

QString InputShapeMask::source() const
{
    return m_source;
}

QColor InputShapeMask::color() const
{
    return m_color;
}

QPointF InputShapeMask::position() const
{
    return m_position;
}

bool InputShapeMask::enabled() const
{
    return m_enabled;
}

QBitmap InputShapeMask::shape() const
{
    return m_shape;
}

void InputShapeMask::setSource(const QString &source)
{
    if (m_source != source) {
        m_source = source;
        Q_EMIT sourceChanged(m_source);
        updateShape();
    }
}

void InputShapeMask::setColor(const QColor &color)
{
    if (m_color != color) {
        m_color = color;
        Q_EMIT colorChanged(m_color);
        updateShape();
    }
}

void InputShapeMask::setPosition(const QPointF &position)
{
    if (m_position != position) {
        m_position = position;
        Q_EMIT positionChanged();
    }
}

void InputShapeMask::setEnabled(bool enabled)
{
    if (m_enabled != enabled) {
        m_enabled = enabled;
        Q_EMIT enabledChanged();
    }
}

#include "inputshapemask.moc"
