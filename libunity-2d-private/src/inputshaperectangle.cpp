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

#include "inputshaperectangle.h"
#include "inputshapemask.h"

#include <QBitmap>
#include <QPainter>
#include <QPainterPath>
#include <QDebug>
#include <QRect>

InputShapeRectangle::InputShapeRectangle(QObject *parent) :
    QObject(parent),
    m_enabled(true),
    m_mirrorHorizontally(false)
{
}

void InputShapeRectangle::updateShape()
{
    QBitmap newShape(m_rectangle.width(), m_rectangle.height());
    newShape.fill(Qt::color1);

    if (!m_rectangle.isEmpty() && m_masks.count() > 0) {
        QPainter painter(&newShape);
        painter.setBackgroundMode(Qt::OpaqueMode);

        Q_FOREACH (InputShapeMask* mask, m_masks) {
            if (mask->enabled()) {
                painter.drawPixmap(mask->position(), mask->shape());
            }
        }
    }

    if (m_mirrorHorizontally) {
        newShape = QBitmap::fromImage(newShape.toImage().mirrored(true, false));
    }

    m_shape = newShape;
    Q_EMIT shapeChanged();
}

QRectF InputShapeRectangle::rectangle() const
{
    return m_rectangle;
}

void InputShapeRectangle::setRectangle(QRectF rectangle)
{
    if (rectangle != m_rectangle) {
        m_rectangle = rectangle;
        updateShape();
        Q_EMIT rectangleChanged();
    }
}

bool InputShapeRectangle::enabled() const
{
    return m_enabled;
}

void InputShapeRectangle::setEnabled(bool enabled)
{
    if (enabled != m_enabled) {
        m_enabled = enabled;
        Q_EMIT enabledChanged();

    }
}

QBitmap InputShapeRectangle::shape() const
{
    return m_shape;
}

QDeclarativeListProperty<InputShapeMask> InputShapeRectangle::masks()
{
    return QDeclarativeListProperty<InputShapeMask>(this, this, &InputShapeRectangle::appendMask);
}

bool InputShapeRectangle::mirrorHorizontally() const
{
    return m_mirrorHorizontally;
}

void InputShapeRectangle::setMirrorHorizontally(bool mirror)
{
    if (mirror != m_mirrorHorizontally) {
        m_mirrorHorizontally = mirror;
        updateShape();
        Q_EMIT mirrorHorizontallyChanged(m_mirrorHorizontally);
    }
}

void InputShapeRectangle::appendMask(QDeclarativeListProperty<InputShapeMask> *list, InputShapeMask *mask)
{
    InputShapeRectangle* instance = qobject_cast<InputShapeRectangle*>(list->object);
    if (instance != NULL) {
        instance->m_masks.append(mask);
        instance->connect(mask, SIGNAL(enabledChanged()), SLOT(updateShape()));
        instance->connect(mask, SIGNAL(shapeChanged()), SLOT(updateShape()));
        instance->connect(mask, SIGNAL(positionChanged()), SLOT(updateShape()));
        instance->updateShape();
    }
}

#include "inputshaperectangle.moc"
