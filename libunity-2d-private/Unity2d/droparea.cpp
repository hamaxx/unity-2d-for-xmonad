/*
 * Copyright (C) 2011 Canonical, Ltd.
 *
 * Authors:
 *  Olivier Tilloy <olivier.tilloy@canonical.com>
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

/* This code is largely inspired by Grégory Schlomoff’s qml-drag-drop project
   (https://bitbucket.org/gregschlom/qml-drag-drop/).
   FIXME: what is the license of the original source code? */

#include "droparea.h"
#include "dragdropevent.h"

DeclarativeDropArea::DeclarativeDropArea(QDeclarativeItem* parent)
    : QDeclarativeItem(parent)
{
    setAcceptDrops(true);
}

void DeclarativeDropArea::dragEnterEvent(QGraphicsSceneDragDropEvent* event)
{
    DeclarativeDragDropEvent dde(event, this);
    Q_EMIT dragEnter(&dde);
}

void DeclarativeDropArea::dragLeaveEvent(QGraphicsSceneDragDropEvent* event)
{
    DeclarativeDragDropEvent dde(event, this);
    Q_EMIT dragLeave(&dde);
}

void DeclarativeDropArea::dropEvent(QGraphicsSceneDragDropEvent* event)
{
    DeclarativeDragDropEvent dde(event, this);
    Q_EMIT drop(&dde);
}

