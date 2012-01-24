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

/* Original ideas for drag’n’drop implementation by Grégory Schlomoff
   (https://bitbucket.org/gregschlom/qml-drag-drop/). */

#ifndef DeclarativeDragDropEvent_H
#define DeclarativeDragDropEvent_H

#include "mimedata.h"

#include <QtCore/Qt>

class QGraphicsSceneDragDropEvent;

class DeclarativeDragDropEvent : public QObject
{
    Q_OBJECT

    Q_PROPERTY(DeclarativeMimeData* mimeData READ mimeData)
    Q_PROPERTY(bool accepted READ accepted WRITE setAccepted)
    Q_PROPERTY(Qt::DropAction dropAction READ dropAction WRITE setDropAction)

public:
    DeclarativeDragDropEvent(QGraphicsSceneDragDropEvent* event, QObject* parent=0);

    /* getters */
    DeclarativeMimeData* mimeData() { return &m_mimeData; }
    bool accepted() const;
    Qt::DropAction dropAction() const;

    /* setters */
    void setAccepted(bool accepted);
    void setDropAction(Qt::DropAction action);

private:
    QGraphicsSceneDragDropEvent* m_event;
    DeclarativeMimeData m_mimeData;
};

Q_DECLARE_METATYPE(DeclarativeDragDropEvent*)

#endif // DeclarativeDragDropEvent

