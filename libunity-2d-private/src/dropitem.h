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

#ifndef DROPITEM_H
#define DROPITEM_H

#include <QDeclarativeItem>

class QGraphicsSceneDragDropEvent;
class DeclarativeDragDropEvent;

class DeclarativeDropItem : public QDeclarativeItem
{
    Q_OBJECT

public:
    DeclarativeDropItem(QDeclarativeItem *parent=0);

Q_SIGNALS:
    void dragEnter(DeclarativeDragDropEvent* event);
    void dragLeave(DeclarativeDragDropEvent* event);
    void drop(DeclarativeDragDropEvent* event);

protected:
    void dragEnterEvent(QGraphicsSceneDragDropEvent *event);
    void dragLeaveEvent(QGraphicsSceneDragDropEvent *event);
    void dropEvent(QGraphicsSceneDragDropEvent *event);
};

#endif // DROPITEM_H
