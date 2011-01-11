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

#ifndef LauncherControl_H
#define LauncherControl_H

#include <QtCore/QObject>
#include <QtDBus/QDBusContext>
#include <QtDeclarative/qdeclarative.h>

class LauncherControl : public QObject, protected QDBusContext
{
    Q_OBJECT

public:
    explicit LauncherControl(QObject* parent=0);
    ~LauncherControl();

    bool connectToBus();

public Q_SLOTS:
    Q_NOREPLY void AddWebFavorite(const QString& url);

Q_SIGNALS:
    void addWebFavorite(const QString& url);
};

QML_DECLARE_TYPE(LauncherControl)

#endif // LauncherControl_H

