/*
 * Copyright (C) 2011 Canonical, Ltd.
 *
 * Authors:
 *  Renato Araujo Oliveira Filho <renato.filho@canonical.com>
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

#include "focuspath.h"

/*!
    \qmlclass FocusPath
    \brief FocusPath is used to help keyboard navigation between QML elements

    The following example shows how to use FocusPath no navigate through a QML Grid element cells.

    \qml
    FocusPath {
        item: myGrid
        columns: myGrid.columns
    }

    Grid {
        id: myGrid
        columns: 4
        Repeater {
            model: myModel
            FocusScope {
                Focupath.index: index
                Rectangle {
                    width: 100
                    height: 100
                    color: activeFocus ? "red" : "blue"
                }
            }
        }
    }
    \endqml

    The FocusPath class should be used with the attached property 'index' to determiner the focus path order.
*/

/*!
    \qmlproperty Component item

    The parent item for navigation
*/

/*!
    \qmlproperty int columns

    Number of columns in the item element, this will allow vertical naviagtion
*/

/*!
    \qmlproperty int index

    Attached property used to determine the focus sequence
*/

/*!
    \qmlproperty bool skip

    Attached property to remove item from the focus path
*/

FocusPath::FocusPath(QObject *parent)
    : QObject(parent),
      m_item(0),
      m_columns(0),
      m_currentIndex(0)
{
}

FocusPath::~FocusPath()
{
    updateItem(NULL);
}

QDeclarativeItem* FocusPath::item() const
{
    return m_item;
}

int FocusPath::columns() const
{
    return m_columns;
}

int FocusPath::currentIndex() const
{
    return m_currentIndex;
}

QDeclarativeItem* FocusPath::currentItem() const
{
    if (m_currentIndex >= 0 && m_currentIndex < m_path.size()) {
        return m_path[m_currentIndex].second;
    }

    return 0;
}

void FocusPath::setItem(QDeclarativeItem* item)
{
    if (m_item != item) {
        updateItem(item);
        Q_EMIT itemChanged();
    }
}

void FocusPath::setColumns(int columns)
{
    if (m_columns != columns) {
        m_columns = columns;
        Q_EMIT columnsChanged();
    }
}

void FocusPath::setCurrentIndex(int index)
{
    if (m_currentIndex != index) {
        QDeclarativeItem* focus = m_path[index].second;
        Q_ASSERT(focus);
        focus->forceActiveFocus();
        m_currentIndex = index;
        Q_EMIT currentIndexChanged();
        Q_EMIT currentItemChanged();
    }
}

void FocusPath::onItemChanged()
{
    FocusPathAttached *info = qobject_cast<FocusPathAttached *> (sender());
    QDeclarativeItem *item = qobject_cast<QDeclarativeItem *>(info->parent());

    removeItem(item);
    addItem(item);
}

/*!
  Reset focus, moving the current focus to the first element
*/

void FocusPath::reset()
{
    if (m_path.size() > 0) {
        QGraphicsItem* gi = m_path[0].second;
        if (gi->flags() & QGraphicsItem::ItemIsFocusScope) {
            gi->setFocus();
            m_path[0].second->setFocus(true);
        }
    }
    m_currentIndex = 0;
    Q_EMIT currentIndexChanged();
    Q_EMIT currentItemChanged();
}

void FocusPath::addItem(QDeclarativeItem *item)
{
    if (item->flags() & QGraphicsItem::ItemIsFocusScope) {
        QObject *attached = qmlAttachedPropertiesObject<FocusPath>(item);
        FocusPathAttached *info = static_cast<FocusPathAttached *>(attached);

        if (!info->skip()) {
            QList<PathItem>::iterator i = m_path.begin();
            for(; i != m_path.end(); i++) {
                if (info->index() < (*i).first) {
                    break;
                }
            }

            if (i == m_path.begin()) {
                m_path.prepend(qMakePair(info->index(), item));
            } else if (i == m_path.end()) {
                m_path.append(qMakePair(info->index(), item));
            } else {
                m_path.insert(--i, qMakePair(info->index(), item));
            }

            reset();
        }

        m_items << item;
        QObject::connect(info, SIGNAL(indexChanged()), this, SLOT(onItemChanged()));
        QObject::connect(info, SIGNAL(skipChanged()), this, SLOT(onItemChanged()));
    }
}

void FocusPath::removeItem(QDeclarativeItem *item)
{
    QList<PathItem>::iterator i = m_path.begin();
    for(int index=0; i != m_path.end(); i++, index++) {
        if ((*i).second == item) {
            m_currentIndex = 0;
            m_path.erase(i);
            reset();
            break;
        }
    }

    m_items.removeOne(item);

    QObject *attached = qmlAttachedPropertiesObject<FocusPath>(item);
    FocusPathAttached *info = static_cast<FocusPathAttached *>(attached);

    if (info) {
        QObject::disconnect(info, SIGNAL(indexChanged()), this, SLOT(onItemChanged()));
        QObject::disconnect(info, SIGNAL(skipChanged()), this, SLOT(onItemChanged()));
    }
}

bool FocusPath::eventFilter(QObject* obj, QEvent* event)
{
    if (obj == m_item) {
        switch(event->type()) {
            case QEvent::KeyPress: {
                int nextFocus = m_currentIndex;

                QKeyEvent *keyEvent = static_cast<QKeyEvent*>(event);
                switch(keyEvent->key()) {
                    case Qt::Key_Right:
                        nextFocus++;
                        break;
                    case Qt::Key_Left:
                        nextFocus--;
                        break;
                    case Qt::Key_Up:
                        if (m_columns >= 0) {
                            nextFocus-= m_columns;
                        } else {
                            nextFocus = -1;
                        }
                        break;
                    case Qt::Key_Down:
                        if (m_columns >= 0) {
                            nextFocus += m_columns;
                        } else {
                            nextFocus = -1;
                        }
                        break;
                    default:
                        nextFocus = -1;
                        break;
                }

                if ((nextFocus >= 0) && (nextFocus < m_path.size())) {
                    setCurrentIndex(nextFocus);
                    return true;
                }
                break;
            }
            default:
               break;
        }
    }
    return false;
}

void FocusPath::onChildrenChanged()
{
    QList<QGraphicsItem *> items = m_item->childItems();

    Q_FOREACH(QDeclarativeItem *i, m_items) {
        QDeclarativeItem *di = qobject_cast<QDeclarativeItem *>(i);
        if (!items.contains(di)) {
            removeItem(i);
        }
    }

    Q_FOREACH(QGraphicsItem *i, items) {
        QGraphicsObject *obj = i->toGraphicsObject();
        if (obj) {
            QDeclarativeItem *di = qobject_cast<QDeclarativeItem *>(obj);
            if (!m_items.contains(di)) {
                addItem(di);
            }
        }
    }
}

void FocusPath::updateItem(QDeclarativeItem* item)
{
    if (item == m_item)
        return;

    if (m_item) {
        m_item->removeEventFilter(this);
        Q_FOREACH(QGraphicsItem *c, m_item->childItems()) {
            QGraphicsObject *obj = c->toGraphicsObject();
            if (obj) {
                QDeclarativeItem *ci = qobject_cast<QDeclarativeItem *>(obj);
                removeItem(ci);
            }
        }
        Q_ASSERT(m_item);
        QObject::disconnect(m_item, SIGNAL(destroyed(QObject*)), this, SLOT(onItemDestroyed()));
        QObject::disconnect(m_item, SIGNAL(childrenChanged()), this, SLOT(onChildrenChanged()));
    }
    m_item = item;
    if (m_item) {
        Q_FOREACH(QGraphicsItem *c, m_item->childItems()) {
            QGraphicsObject *obj = c->toGraphicsObject();
            if (obj) {
                QDeclarativeItem *ci = qobject_cast<QDeclarativeItem *>(obj);
                addItem(ci);
            }
        }
        m_item->installEventFilter(this);
        QObject::connect(m_item, SIGNAL(destroyed(QObject*)), this, SLOT(onItemDestroyed()));
        QObject::connect(m_item, SIGNAL(childrenChanged()), this, SLOT(onChildrenChanged()));
    }
}

void FocusPath::onItemDestroyed()
{
    Q_FOREACH(QDeclarativeItem *i, m_items) {
        removeItem(i);
    }
    m_item = 0;
}

FocusPathAttached *FocusPath::qmlAttachedProperties(QObject *object)
{
    return new FocusPathAttached(object);
}

FocusPathAttached::FocusPathAttached(QObject *object)
    : QObject(object),
      m_index(0),
      m_skip(false)
{
}

int FocusPathAttached::index() const
{
    return m_index;
}

bool FocusPathAttached::skip() const
{
    return m_skip;
}

void FocusPathAttached::setIndex(int index)
{
    if (m_index != index) {
        m_index = index;
        Q_EMIT indexChanged();
    }
}

void FocusPathAttached::setSkip(bool skip)
{
    if (m_skip != skip) {
        m_skip = skip;
        Q_EMIT skipChanged();
    }
}

#include "focuspath.moc"
