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

#ifndef INPUTSHAPEMASK_H
#define INPUTSHAPEMASK_H

#include <QObject>
#include <QColor>
#include <QPoint>
#include <QBitmap>

class InputShapeMask : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString source READ source WRITE setSource NOTIFY sourceChanged)
    Q_PROPERTY(QColor color READ color WRITE setColor NOTIFY colorChanged)
    Q_PROPERTY(QPointF position READ position WRITE setPosition NOTIFY positionChanged)
    Q_PROPERTY(QBitmap shape READ shape NOTIFY shapeChanged)
    Q_PROPERTY(bool enabled READ enabled WRITE setEnabled NOTIFY enabledChanged)

public:
    explicit InputShapeMask(QObject *parent = 0);

    QString source() const;
    QColor color() const;
    QPointF position() const;
    bool enabled() const;
    QBitmap shape() const;

    void setSource(const QString& source);
    void setColor(const QColor& color);
    void setPosition(const QPointF& position);
    void setEnabled(bool enabled);

Q_SIGNALS:
    void sourceChanged(const QString& source);
    void colorChanged(const QColor& color);
    void enabledChanged();
    void shapeChanged();
    void positionChanged();

protected:
    void updateShape();

private:
    QString m_source;
    QColor m_color;
    QPointF m_position;
    bool m_enabled;
    QBitmap m_shape;
};

#endif // INPUTSHAPEMASK_H
