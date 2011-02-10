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

#ifndef DeclarativeDragDropEvent_H
#define DeclarativeDragDropEvent_H

#include "mimedata.h"

#include <QtGui/QGraphicsSceneDragDropEvent>

class DeclarativeDragDropEvent : public QObject
{
    Q_OBJECT

    Q_PROPERTY(DeclarativeMimeData* mimeData READ mimeData)

public:
    DeclarativeDragDropEvent(QGraphicsSceneDragDropEvent* event, QObject* parent=0);

    /* getters */
    DeclarativeMimeData* mimeData() { return &m_mimeData; }

    Q_INVOKABLE void accept();
    Q_INVOKABLE void acceptProposedAction();

private:
    QGraphicsSceneDragDropEvent* m_event;
    DeclarativeMimeData m_mimeData;
};

#endif // DeclarativeDragDropEvent

