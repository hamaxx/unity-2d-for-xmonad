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

#include "dragdropevent.h"

#include <QGraphicsSceneDragDropEvent>

DeclarativeDragDropEvent::DeclarativeDragDropEvent(QGraphicsSceneDragDropEvent* event, QObject* parent)
    : QObject(parent)
    , m_event(event)
    , m_mimeData(event->mimeData())
{
    /* By default the event is not accepted, let the receiver decide. */
    m_event->setAccepted(false);
}

bool
DeclarativeDragDropEvent::accepted() const
{
    return m_event->isAccepted();
}

void
DeclarativeDragDropEvent::setAccepted(bool accepted)
{
    m_event->setAccepted(accepted);
}

Qt::DropAction
DeclarativeDragDropEvent::dropAction() const
{
    return m_event->dropAction();
}

void
DeclarativeDragDropEvent::setDropAction(Qt::DropAction action)
{
    m_event->setDropAction(action);
}

#include "dragdropevent.moc"

