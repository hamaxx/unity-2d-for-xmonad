/*
 * Copyright (C) 2010 Canonical, Ltd.
 *
 * Authors:
 *  Ugo Riboni <ugo.riboni@canonical.com>
 *  Florian Boucault <florian.boucault@canonical.com>
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

#ifndef SPREADCONTROL_H
#define SPREADCONTROL_H

#include <QObject>
#include <QDBusContext>
#include <QtDeclarative/qdeclarative.h>
#include <QWidget>

class LauncherClient;

class SpreadControl : public QObject, protected QDBusContext
{
    Q_OBJECT

public:
    explicit SpreadControl(QObject *parent = 0);
    ~SpreadControl();

    bool connectToBus();

public Q_SLOTS:
    Q_NOREPLY void ShowAllWorkspaces(QString applicationDesktopFile);
    Q_NOREPLY void ShowCurrentWorkspace(QString applicationDesktopFile);
    Q_NOREPLY void FilterByApplication(QString applicationDesktopFile);
    Q_NOREPLY void Hide();
    bool IsShown() { return m_isShown; }

private Q_SLOTS:
    void setIsShown(bool isShown);

Q_SIGNALS:
    void showAllWorkspaces(QString applicationDesktopFile);
    void showCurrentWorkspace(QString applicationDesktopFile);
    void filterByApplication(QString applicationDesktopFile);
    void hide();

private:
    bool m_isShown;
    LauncherClient *m_launcherClient;
};

QML_DECLARE_TYPE(SpreadControl)

#endif // SPREADCONTROL_H
