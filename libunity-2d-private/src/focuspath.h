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

#ifndef FOCUSPATH_H
#define FOCUSPATH_H

#include <QObject>
#include <QDeclarativeItem>

typedef QPair<int, QDeclarativeItem *> PathItem;

class FocusPathAttached;

class FocusPath : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QDeclarativeItem* item READ item WRITE setItem NOTIFY itemChanged)
    Q_PROPERTY(int columns READ columns WRITE setColumns NOTIFY columnsChanged)
    Q_PROPERTY(int currentIndex READ currentIndex WRITE setCurrentIndex NOTIFY currentIndexChanged)
    Q_PROPERTY(QDeclarativeItem* currentItem READ currentItem NOTIFY currentItemChanged)
    Q_PROPERTY(Flow flow READ flow WRITE setFlow NOTIFY flowChanged)
    Q_PROPERTY(NavigationDirection direction READ direction WRITE setDirection NOTIFY directionChanged)

public:
    explicit FocusPath(QObject *parent = 0);
    ~FocusPath();

    Q_ENUMS(Flow)
    enum Flow {
        LeftToRight,
        TopToBottom,
        RightToLeft
    };

    Q_ENUMS(NavigationDirection)
    enum NavigationDirection {
        Horizontal              = 0x001,
        Vertical                = 0x010,
        HorizontalAndVertical   = Horizontal | Vertical
    };

    /* getters */
    int columns() const;
    QDeclarativeItem *item() const;
    int currentIndex() const;
    QDeclarativeItem *currentItem() const;
    Flow flow() const;
    NavigationDirection direction() const;

    /* setters */
    void setColumns(int columns);
    void setItem(QDeclarativeItem *item);
    void setCurrentIndex(int index);
    void setFlow(Flow value);
    void setDirection(NavigationDirection value);

    QList<PathItem > path() const;

    Q_INVOKABLE void reset();
    Q_INVOKABLE void focusLastRow();

    Q_INVOKABLE bool moveToNext();
    Q_INVOKABLE bool moveToPrevious();

    Q_INVOKABLE int previousIndex();

    static FocusPathAttached *qmlAttachedProperties(QObject *object);

Q_SIGNALS:
    void itemChanged();
    void columnsChanged();
    void currentIndexChanged();
    void currentItemChanged();
    void flowChanged();
    void directionChanged();

protected Q_SLOTS:
    void onChildrenChanged();
    void onItemChanged();
    void onInfoChanged();
    void onItemDestroyed();
    void addItem(QDeclarativeItem *item);
    void removeItem(QDeclarativeItem *item);

protected:
    bool eventFilter(QObject *obj, QEvent *event);

private:
    QDeclarativeItem *m_item;
    int m_columns;
    int m_rows;
    int m_currentPosition;
    Flow m_flow;
    NavigationDirection m_direction;
    QList<PathItem > m_path;
    QList<QDeclarativeItem *> m_items;


    void updateItem(QDeclarativeItem *item);
    void updatePosition(int newPosition);
};

class FocusPathAttached : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int index READ index WRITE setIndex NOTIFY indexChanged)
    Q_PROPERTY(bool skip READ skip WRITE setSkip NOTIFY skipChanged)

public:
    FocusPathAttached(QObject *object);

    int index() const;
    bool skip() const;

    void setIndex(int index);
    void setSkip(bool skip);

Q_SIGNALS:
    void indexChanged();
    void skipChanged();

private:
    int m_index;
    bool m_skip;

    friend class FocusPath;
};

QML_DECLARE_TYPEINFO(FocusPath, QML_HAS_ATTACHED_PROPERTIES)

#endif // FOCUSPATH_H
