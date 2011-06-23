/*
 * This file is part of unity-2d
 *
 * Copyright 2011 Canonical Ltd.
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

#ifndef UNITYCOREAPPLET_H
#define UNITYCOREAPPLET_H

// Local
#include <applet.h>

// libunity-core
#include <UnityCore/UnityCore.h>

class UnityCoreApplet : public Unity2d::Applet
{
Q_OBJECT
public:
    UnityCoreApplet();

private:
    Q_DISABLE_COPY(UnityCoreApplet)
    unity::indicator::DBusIndicators::Ptr m_indicators;

    void onObjectAdded(unity::indicator::Indicator::Ptr const&);
    void onEntryShowMenu(const std::string&, int x, int y, int timestamp, int button);
};

#endif /* UNITYCOREAPPLET_H */
