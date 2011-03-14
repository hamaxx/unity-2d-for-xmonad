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

#ifndef LauncherDBus_H
#define LauncherDBus_H

#include <QtCore/QObject>
#include <QtDBus/QDBusContext>
#include <QtDeclarative/qdeclarative.h>

class VisibilityController;

/**
 * DBus interface for the launcher.
 *
 * Note: BeginForceVisible() and EndForceVisible() should not be called from
 * within the Launcher. Use VisibilityController::beginForceVisible() and
 * VisibilityController::endForceVisible() instead.
 *
 */
class LauncherDBus : public QObject, protected QDBusContext
{
    Q_OBJECT

public:
    explicit LauncherDBus(VisibilityController* visibilityController, QObject* parent=0);
    ~LauncherDBus();

    bool connectToBus();

public Q_SLOTS:
    Q_NOREPLY void BeginForceVisible();
    Q_NOREPLY void EndForceVisible();
    Q_NOREPLY void AddWebFavorite(const QString& url);

Q_SIGNALS:
    void addWebFavorite(const QString& url);

private:
    VisibilityController* m_visibilityController;
};

QML_DECLARE_TYPE(LauncherDBus)

#endif // LauncherDBus_H

