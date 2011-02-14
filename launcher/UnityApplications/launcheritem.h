/*
 * Copyright (C) 2010 Canonical, Ltd.
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

#ifndef LAUNCHERITEM_H
#define LAUNCHERITEM_H

#include <QObject>
#include <QString>

#include "dragdropevent.h"

class LauncherContextualMenu;

class LauncherItem : public QObject
{
    Q_OBJECT

    Q_PROPERTY(bool active READ active NOTIFY activeChanged)
    Q_PROPERTY(bool running READ running NOTIFY runningChanged)
    Q_PROPERTY(bool urgent READ urgent NOTIFY urgentChanged)
    Q_PROPERTY(QString name READ name NOTIFY nameChanged)
    Q_PROPERTY(QString icon READ icon NOTIFY iconChanged)
    Q_PROPERTY(bool launching READ launching NOTIFY launchingChanged)
    /* Export the menu as a plain QObject so that it can be used from QML */
    Q_PROPERTY(QObject* menu READ menu NOTIFY menuChanged)

public:
    LauncherItem(QObject* parent = 0);
    ~LauncherItem();

    /* getters */
    virtual bool active() const = 0;
    virtual bool running() const = 0;
    virtual bool urgent() const = 0;
    virtual QString name() const = 0;
    virtual QString icon() const = 0;
    virtual bool launching() const = 0;
    QObject* menu() const;

    /* methods */
    Q_INVOKABLE virtual void activate() = 0;
    Q_INVOKABLE virtual void createMenuActions() = 0;

protected:
    LauncherContextualMenu* m_menu;

signals:
    void activeChanged(bool);
    void runningChanged(bool);
    void urgentChanged(bool);
    void nameChanged(QString);
    void iconChanged(QString);
    void launchingChanged(bool);
    void menuChanged(QObject*);

public Q_SLOTS:
    /* Default implementation of drag’n’drop handling, should be overridden in
       subclasses to implement custom behaviours. The handlers should return
       true if they can handle the event, independently of whether they actually
       succeed to handle it (which is tracked by setting the 'accepted' property
       of the event to true). */
    /* The 'event' parameters should be DeclarativeDragDropEvent*, but because
       of http://bugreports.qt.nokia.com/browse/QTBUG-13047 they need to be
       passed around from QML to C++ as QObject*. This is fixed in Qt 4.7.1. */
    virtual bool onDragEnter(QObject* event);
    virtual bool onDrop(QObject* event);
};

#endif // LAUNCHERITEM_H

