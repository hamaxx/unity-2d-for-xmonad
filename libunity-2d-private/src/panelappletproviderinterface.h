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

#ifndef PANELAPPLETPROVIDERINTERFACE_H
#define PANELAPPLETPROVIDERINTERFACE_H

#include "panelapplet.h"

namespace Unity2d
{

class PanelAppletProviderInterface
{
public:
    virtual QString appletName() const = 0;
    virtual PanelApplet* createApplet(Unity2dPanel* panel) const = 0;
};

} // namespace Unity2d

Q_DECLARE_INTERFACE(Unity2d::PanelAppletProviderInterface,
                    "com.canonical.Unity2d.PanelAppletProviderInterface/1.0")

#endif // PANELAPPLETPROVIDERINTERFACE_H
