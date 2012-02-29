/*
 * This file is part of unity-2d
 *
 * Copyright 2012 Canonical Ltd.
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

#ifndef CURSORSHAPEAREA_H
#define CURSORSHAPEAREA_H

#include <QDeclarativeItem>

class CursorShapeArea : public QDeclarativeItem
{
    Q_OBJECT

    Q_PROPERTY(Qt::CursorShape cursorShape READ cursorShape WRITE setCursorShape NOTIFY cursorShapeChanged)

public:
    explicit CursorShapeArea(QDeclarativeItem *parent = 0);

    Qt::CursorShape cursorShape() const;
    void setCursorShape(Qt::CursorShape cursorShape);

private:
    Q_DISABLE_COPY(CursorShapeArea)
    Qt::CursorShape m_currentShape;

Q_SIGNALS:
    void cursorShapeChanged();
};

#endif // CURSORSHAPEAREA_H
