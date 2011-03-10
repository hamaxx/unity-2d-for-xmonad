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

#ifndef TRASH_H
#define TRASH_H

#include <gio/gio.h>

#include "launcheritem.h"

#include <QAbstractListModel>
#include <QMetaType>

class Trash : public LauncherItem
{
    Q_OBJECT

public:
    Trash();
    Trash(const Trash& other);
    ~Trash();

    /* getters */
    virtual bool active() const;
    virtual bool running() const;
    virtual int windowCount() const;
    virtual bool urgent() const;
    virtual QString name() const;
    virtual QString icon() const;
    virtual bool launching() const;

    /* methods */
    Q_INVOKABLE virtual void activate();
    Q_INVOKABLE virtual void createMenuActions();

public Q_SLOTS:
    /* Custom implementation of drag’n’drop handling. Dropping files on the
       trash will… move them to the trash! */
    void onDragEnter(DeclarativeDragDropEvent*);
    void onDrop(DeclarativeDragDropEvent*);

private Q_SLOTS:
    void onEmptyTriggered();

private:
    void open() const;
    void empty() const;
    int count() const;

    static void recursive_delete(GFile* dir);

    GFile* m_trash;
};

Q_DECLARE_METATYPE(Trash*)


class Trashes : public QAbstractListModel
{
    Q_OBJECT

public:
    Trashes(QObject* parent = 0);
    ~Trashes();

    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;
    int rowCount(const QModelIndex& parent = QModelIndex()) const;

private:
    Trash* m_trash;
};

#endif // TRASH_H

