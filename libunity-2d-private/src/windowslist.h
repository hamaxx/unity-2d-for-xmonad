/*
 * Copyright (C) 2010 Canonical, Ltd.
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

#ifndef WINDOWSLIST_H
#define WINDOWSLIST_H

#include <QAbstractListModel>
#include <QVariant>
#include <QObject>
#include <QtDeclarative/qdeclarative.h>

class WindowInfo;
class BamfView;

/* FIXME: this should be update dynamically whenever new windows are opened
   or go away. Both wnck and bamf have signals for this */
class WindowsList : public QAbstractListModel
{
    Q_OBJECT

public:
    WindowsList(QObject *parent = 0);
    ~WindowsList();

    QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;
    int rowCount(const QModelIndex & parent = QModelIndex()) const;
    Q_INVOKABLE virtual bool removeRows(int row, int count,
                                        const QModelIndex& parent = QModelIndex());

    Q_INVOKABLE void load();
    Q_INVOKABLE void unload();

public Q_SLOTS:
    void addWindow(BamfView *view);
    void removeWindow(BamfView *view);
    void updateWorkspaceRole(int workspace);

protected:
    QList<WindowInfo*> m_windows;
};

QML_DECLARE_TYPE(WindowsList)

#endif // WINDOWSLIST_H
