/*
 * This file is part of unity-2d
 *
 * Copyright 2010 Canonical Ltd.
 *
 * Authors:
 * - Aurélien Gâteau <aurelien.gateau@canonical.com>
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

#ifndef HOMEBUTTONAPPLET_H
#define HOMEBUTTONAPPLET_H

// Local
#include "homebutton.h"

// Unity-2d
#include <panelapplet.h>

class QDBusInterface;
class LauncherClient;

class HomeButtonApplet : public Unity2d::PanelApplet
{
Q_OBJECT
public:
    HomeButtonApplet(Unity2dPanel* panel);

protected:
    void enterEvent(QEvent*);
    void leaveEvent(QEvent*);

private Q_SLOTS:
    void toggleDash();
    void connectToDash();

private:
    Q_DISABLE_COPY(HomeButtonApplet)
    HomeButton* m_button;
    QDBusInterface* m_dashInterface;
    LauncherClient* m_launcherClient;
};

#endif /* HOMEBUTTONAPPLET_H */
