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

#include "dragitem.h"

#include <QDeclarativeEngine>
#include <QDeclarativeComponent>
#include <QGraphicsSceneMouseEvent>
#include <QApplication>
#include <QDrag>
#include <QGraphicsScene>
#include <QPainter>
#include <QMimeData>
#include <QX11Info>

DeclarativeDragItem::DeclarativeDragItem(QDeclarativeItem* parent)
    : QDeclarativeItem(parent)
    , m_delegate(NULL)
    , m_supportedActions(Qt::LinkAction)
    , m_defaultAction(Qt::LinkAction)
{
    setAcceptedMouseButtons(Qt::LeftButton);
}

DeclarativeDragItem::~DeclarativeDragItem()
{
}

QDeclarativeComponent*
DeclarativeDragItem::delegate() const
{
    return m_delegate;
}

void
DeclarativeDragItem::setDelegate(QDeclarativeComponent* delegate)
{
    if (delegate != m_delegate) {
        m_delegate = delegate;
        Q_EMIT delegateChanged();
    }
}

void
DeclarativeDragItem::resetDelegate()
{
    setDelegate(NULL);
}

Qt::DropActions
DeclarativeDragItem::supportedActions() const
{
    return m_supportedActions;
}

void
DeclarativeDragItem::setSupportedActions(Qt::DropActions actions)
{
    if (actions != m_supportedActions) {
        m_supportedActions = actions;
        Q_EMIT supportedActionsChanged();
    }
}

Qt::DropAction
DeclarativeDragItem::defaultAction() const
{
    return m_defaultAction;
}

void
DeclarativeDragItem::setDefaultAction(Qt::DropAction action)
{
    if (action != m_defaultAction) {
        m_defaultAction = action;
        Q_EMIT defaultActionChanged();
    }
}

QMimeData* DeclarativeDragItem::mimeData() const
{
    // Default implementation, empty mime data.
    return new QMimeData;
}

void
DeclarativeDragItem::mouseMoveEvent(QGraphicsSceneMouseEvent* event)
{
    int distance = (event->buttonDownScreenPos(Qt::LeftButton) - event->screenPos()).manhattanLength();
    if (distance < QApplication::startDragDistance()) {
        return;
    }

    QDrag* drag = new QDrag(event->widget());
    drag->setMimeData(mimeData());

    if (m_delegate != NULL) {
        QObject* delegateObject = m_delegate->create(QDeclarativeEngine::contextForObject(this));
        QDeclarativeItem* delegate = qobject_cast<QDeclarativeItem*>(delegateObject);
        if (delegate != NULL) {
            /* Render the delegate to a pixmap. */
            QGraphicsScene scene;
            scene.addItem(delegate);

            QPixmap pixmap(scene.sceneRect().width(), scene.sceneRect().height());
            bool compositing = QX11Info::isCompositingManagerRunning();
            if (!compositing) {
                pixmap.fill(Qt::transparent);
            }
            QPainter painter(&pixmap);
            if (compositing) {
                painter.setCompositionMode(QPainter::CompositionMode_Source);
            } else {
                /* Cheap solution to avoid aliasing: draw a solid white background. */
                painter.setPen(Qt::white);
                painter.setBrush(Qt::white);
                painter.drawRoundedRect(scene.sceneRect(), 5, 5);
            }
            scene.render(&painter);
            scene.removeItem(delegate);
            delete delegate;

            drag->setPixmap(pixmap);
            drag->setHotSpot(QPoint(pixmap.width() / 2, pixmap.height() / 2));
        }
    }

    Qt::DropAction action = drag->exec(m_supportedActions, m_defaultAction);
    Q_EMIT drop(action);
}

void
DeclarativeDragItem::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
    Q_EMIT pressed();
}

void
DeclarativeDragItem::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
    Q_EMIT released();
}

#include "dragitem.moc"
