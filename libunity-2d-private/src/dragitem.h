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

#ifndef DeclarativeDragItem_H
#define DeclarativeDragItem_H

#include <QDeclarativeItem>

class QMimeData;
class QDeclarativeComponent;

class DeclarativeDragItem : public QDeclarativeItem
{
    Q_OBJECT

    Q_PROPERTY(QDeclarativeComponent* delegate READ delegate WRITE setDelegate NOTIFY delegateChanged RESET resetDelegate)
    Q_PROPERTY(Qt::DropActions supportedActions READ supportedActions WRITE setSupportedActions NOTIFY supportedActionsChanged)
    Q_PROPERTY(Qt::DropAction defaultAction READ defaultAction WRITE setDefaultAction NOTIFY defaultActionChanged)

public:
    DeclarativeDragItem(QDeclarativeItem* parent=0);
    ~DeclarativeDragItem();

    // getters and setters
    QDeclarativeComponent* delegate() const;
    void setDelegate(QDeclarativeComponent* delegate);
    void resetDelegate();
    Qt::DropActions supportedActions() const;
    void setSupportedActions(Qt::DropActions actions);
    Qt::DropAction defaultAction() const;
    void setDefaultAction(Qt::DropAction action);

protected:
    // override in child classes to set a non-empty mime data
    virtual QMimeData* mimeData() const;

Q_SIGNALS:
    void delegateChanged();
    void supportedActionsChanged();
    void defaultActionChanged();
    void drop(int action);

    // Can’t pass around the mouse events as parameters
    // as QDeclarativeMouseEvent is a private class.
    void pressed();
    void released();

protected:
    // reimplemented
    void mouseMoveEvent(QGraphicsSceneMouseEvent* event);
    void mousePressEvent(QGraphicsSceneMouseEvent* event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent* event);

private:
    QDeclarativeComponent* m_delegate;
    Qt::DropActions m_supportedActions;
    Qt::DropAction m_defaultAction;
};

#endif // DeclarativeDragItem_H

