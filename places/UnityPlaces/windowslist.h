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

/* FIXME: this should be update dynamically whenever new windows are opened
   or go away. Both wnck and bamf have signals for this */
class WindowsList : public QAbstractListModel
{
    Q_OBJECT

    Q_PROPERTY(int count READ count NOTIFY countChanged);

public:
    WindowsList(QObject *parent = 0);
    ~WindowsList();

    QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;
    int rowCount(const QModelIndex & parent = QModelIndex()) const;

    int count() const { return rowCount(); }
    WindowInfo* lastActiveWindow() const;

    Q_INVOKABLE void load();
    Q_INVOKABLE void unload();

signals:
    void countChanged(int count);

private:
    QList<WindowInfo*> m_windows;
    bool m_loaded;
};

QML_DECLARE_TYPE(WindowsList)

#endif // WINDOWSLIST_H
