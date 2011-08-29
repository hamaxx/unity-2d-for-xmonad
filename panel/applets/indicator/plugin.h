/*
 * This file is part of unity-2d
 *
 * Copyright 2011 Canonical Ltd.
 *
 * Authors:
 * - Ugo Riboni <ugo.riboni@canonical.com>
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
 * along with this program.  If not, see <http://www.gnu.org/licenses/>
 */

#ifndef INDICATOR_PLUGIN_H
#define INDICATOR_PLUGIN_H

#include <QString>
#include <panelappletproviderinterface.h>

using namespace Unity2d;

class IndicatorPlugin : public QObject, PanelAppletProviderInterface
{
    Q_OBJECT
    Q_INTERFACES(Unity2d::PanelAppletProviderInterface)

public:
    QString appletName() const;
    PanelApplet* createApplet(Unity2dPanel* panel) const;
};

#endif // INDICATOR_PLUGIN_H
