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

#include "plugin.h"
#include "legacytrayapplet.h"

#include <QtPlugin>

QString LegacyTrayPlugin::appletName() const
{
    return QString("legacytray");
}

PanelApplet* LegacyTrayPlugin::createApplet(Unity2dPanel* panel) const
{
    return new LegacyTrayApplet(panel);
}

Q_EXPORT_PLUGIN2(panelplugin-legacytray, LegacyTrayPlugin)

#include "plugin.moc"
