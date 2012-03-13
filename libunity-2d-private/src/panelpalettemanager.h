/*
 * This file is part of unity-2d
 *
 * Copyright 2012 Canonical Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published
 * by the Free Software Foundation; version 3.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef PANELPALETTEMANAGER_H
#define PANELPALETTEMANAGER_H

// Local
class Unity2dPanel;

// libunity-2d
#include <gconnector.h>

// Qt
#include <QObject>

namespace Unity2d
{

class PanelPaletteManager : public QObject
{
    Q_OBJECT
public:
    explicit PanelPaletteManager(Unity2dPanel* panel);

public Q_SLOTS:
    void updatePalette();

private:
    Q_DISABLE_COPY(PanelPaletteManager)

    Unity2dPanel* m_panel;
    GConnector m_gConnector;
};

} // namespace Unity2d

#endif // PANELPALETTEMANAGER_H
