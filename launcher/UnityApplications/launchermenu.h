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

#ifndef LAUNCHERMENU_H
#define LAUNCHERMENU_H

#include "launcherapplication.h"

#include <QMenu>
#include <QVariant>
#include <QAction>

class QLauncherContextualMenu : public QMenu
{
    Q_OBJECT

public:
    QLauncherContextualMenu(QWidget *parent = 0);
    ~QLauncherContextualMenu();

    Q_INVOKABLE void show(int y, const QVariant& application);
    //Q_INVOKABLE void hide();

private:
    QLauncherApplication* m_application;
    QAction* m_title;
    QAction* m_keep;
    QAction* m_separator;
    QAction* m_quit;

private slots:
    void onKeepTriggered();
    void onQuitTriggered();
};

#endif // LAUNCHERMENU_H
