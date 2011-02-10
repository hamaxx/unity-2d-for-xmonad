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

#ifndef DeclarativeDropArea_H
#define DeclarativeDropArea_H

#include <QtDeclarative/QDeclarativeItem>
#include <QtGui/QGraphicsSceneDragDropEvent>

class DeclarativeDragDropEvent;

class DeclarativeDropArea : public QDeclarativeItem
{
    Q_OBJECT

    Q_PROPERTY(bool acceptDrops READ acceptDrops WRITE setAcceptDrops)

public:
    DeclarativeDropArea(QDeclarativeItem* parent=0);

Q_SIGNALS:
    void dragEnter(DeclarativeDragDropEvent* event);
    void dragLeave(DeclarativeDragDropEvent* event);
    void drop(DeclarativeDragDropEvent* event);

protected:
    void dragEnterEvent(QGraphicsSceneDragDropEvent* event);
    void dragLeaveEvent(QGraphicsSceneDragDropEvent* event);
    void dropEvent(QGraphicsSceneDragDropEvent* event);
};

#endif // DeclarativeDropArea

