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

#ifndef LAUNCHERTOOLTIP_H
#define LAUNCHERTOOLTIP_H

#include "launcherapplication.h"

#include <QMainWindow>
#include <QVariant>
#include <QPropertyAnimation>

class QLauncherTooltip : public QMainWindow
{
    Q_OBJECT

public:
    QLauncherTooltip(QObject *parent = 0);
    ~QLauncherTooltip();

    Q_INVOKABLE void show(int y, const QVariant& application);
    Q_INVOKABLE void hide();
    Q_INVOKABLE void show_menu();

private:
    bool m_menu;
    QLauncherApplication* m_application;
    QPropertyAnimation* m_animation;
};

#endif // LAUNCHERTOOLTIP_H