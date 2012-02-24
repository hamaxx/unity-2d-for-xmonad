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

#ifndef INPUTSHAPERECTANGLE_H
#define INPUTSHAPERECTANGLE_H

#include <QObject>
#include <QRect>
#include <QBitmap>
#include <QDeclarativeListProperty>
#include <QList>

class InputShapeMask;

class InputShapeRectangle : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QRectF rectangle READ rectangle WRITE setRectangle NOTIFY rectangleChanged)
    Q_PROPERTY(bool enabled READ enabled WRITE setEnabled NOTIFY enabledChanged)
    Q_PROPERTY(bool mirrorHorizontally READ mirrorHorizontally WRITE setMirrorHorizontally NOTIFY mirrorHorizontallyChanged)
    Q_PROPERTY(QBitmap shape READ shape NOTIFY shapeChanged)
    Q_PROPERTY(QDeclarativeListProperty<InputShapeMask> masks READ masks)
    Q_CLASSINFO("DefaultProperty", "masks")

public:
    explicit InputShapeRectangle(QObject *parent = 0);

    QRectF rectangle() const;
    void setRectangle(QRectF rectangle);
    bool enabled() const;
    void setEnabled(bool enabled);
    QBitmap shape() const;
    QDeclarativeListProperty<InputShapeMask> masks();
    bool mirrorHorizontally() const;
    void setMirrorHorizontally(bool mirror);

protected:
    static void appendMask(QDeclarativeListProperty<InputShapeMask> *list, InputShapeMask *mask);

protected Q_SLOTS:
    void updateShape();

Q_SIGNALS:
    void rectangleChanged();
    void enabledChanged();
    void shapeChanged();
    void mirrorHorizontallyChanged(bool mirrorHorizontally);

private:
    QRectF m_rectangle;
    bool m_enabled;
    bool m_mirrorHorizontally;
    QBitmap m_shape;
    QList<InputShapeMask*> m_masks;
};

#endif // INPUTSHAPERECTANGLE_H
