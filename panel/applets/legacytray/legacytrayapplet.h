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

#ifndef LEGACYTRAYAPPLET_H
#define LEGACYTRAYAPPLET_H

// Unity-2d
#include <panelapplet.h>

namespace SystemTray
{
class FdoSelectionManager;
class Task;
}

class LegacyTrayApplet : public Unity2d::PanelApplet
{
Q_OBJECT
public:
    LegacyTrayApplet(Unity2dPanel* panel);
    virtual ~LegacyTrayApplet();

private Q_SLOTS:
    void slotTaskCreated(SystemTray::Task*);
    void slotWidgetCreated(QWidget* widget);

private:
    Q_DISABLE_COPY(LegacyTrayApplet)
    
    SystemTray::FdoSelectionManager* m_selectionManager;
    QStringList m_whitelist;
    bool m_whitelistAll;
};

#endif /* LEGACYTRAYAPPLET_H */
