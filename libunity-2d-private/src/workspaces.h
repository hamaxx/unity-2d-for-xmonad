/*
 * Copyright (C) 2011 Canonical, Ltd.
 *
 * Authors:
 *  Ugo Riboni <ugo.riboni@canonical.com>
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

#ifndef WORKSPACES_H
#define WORKSPACES_H

#include <gio/gio.h>

#include "launcheritem.h"

#include <QAbstractListModel>
#include <QMetaType>

class Workspaces : public LauncherItem
{
    Q_OBJECT

public:
    Workspaces();
    Workspaces(const Workspaces& other);
    ~Workspaces();

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
};

Q_DECLARE_METATYPE(Workspaces*)

/* This class wouldn't be necessary if the launcher were able
   to accept single LauncherItems instad of only groups of
   LauncherItems in a list model. */
class WorkspacesList : public QAbstractListModel
{
    Q_OBJECT

public:
    WorkspacesList(QObject* parent = 0);
    ~WorkspacesList();

    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;
    int rowCount(const QModelIndex& parent = QModelIndex()) const;

private:
    Workspaces* m_workspaces;
};

#endif // WORKSPACES_H

