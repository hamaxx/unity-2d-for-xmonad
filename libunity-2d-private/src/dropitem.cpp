/*
 * Copyright (C) 2011 Canonical, Ltd.
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

#include "dropitem.h"

#include <QGraphicsSceneDragDropEvent>

#include "dragdropevent.h"

DeclarativeDropItem::DeclarativeDropItem(QDeclarativeItem *parent) : QDeclarativeItem(parent)
{
    setAcceptDrops(true);
}

void DeclarativeDropItem::dragEnterEvent(QGraphicsSceneDragDropEvent *event)
{
    DeclarativeDragDropEvent dragDropEvent(event, this);
    Q_EMIT dragEnter(&dragDropEvent);
}

void DeclarativeDropItem::dragLeaveEvent(QGraphicsSceneDragDropEvent *event)
{
    DeclarativeDragDropEvent dragDropEvent(event, this);
    Q_EMIT dragLeave(&dragDropEvent);
}

void DeclarativeDropItem::dropEvent(QGraphicsSceneDragDropEvent *event)
{
    DeclarativeDragDropEvent dragDropEvent(event, this);
    Q_EMIT drop(&dragDropEvent);
}

#include "dropitem.moc"
