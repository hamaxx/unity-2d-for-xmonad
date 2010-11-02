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

#include <QMenu>
#include <QAction>

class LauncherContextualMenu : public QMenu
{
    Q_OBJECT

public:
    LauncherContextualMenu();
    ~LauncherContextualMenu();

    Q_INVOKABLE void setTitle(QString title);
    Q_INVOKABLE void prependAction(QAction* action);

private:
    QAction* m_title;

    void loadCSS();
};

#endif // LAUNCHERMENU_H

