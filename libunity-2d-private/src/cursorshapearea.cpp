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

#include "cursorshapearea.h"

CursorShapeArea::CursorShapeArea(QDeclarativeItem *parent)
:   QDeclarativeItem(parent)
,   m_currentShape(Qt::ArrowCursor)
{
}

Qt::CursorShape CursorShapeArea::cursorShape() const
{
  return cursor().shape();
}

void CursorShapeArea::setCursorShape(Qt::CursorShape cursorShape)
{
  if (m_currentShape == cursorShape)
    return;

  setCursor(cursorShape);
  Q_EMIT cursorShapeChanged();

  m_currentShape = cursorShape;
}

#include "cursorshapearea.moc"
