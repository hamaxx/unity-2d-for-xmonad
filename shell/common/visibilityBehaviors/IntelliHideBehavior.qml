/*
 * This file is part of unity-2d
 *
 * Copyright 2012 Canonical Ltd.
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

import QtQuick 1.0
import Unity2d 1.0
import "../utils.js" as Utils

// Shows the target when it has the focus or when you trigger the pointer barrier
// or there are no windows that intersect with the target
// Hides the target when none of the above conditions are met
// and you have not had the mouse over it during more than 1000 msec
// To use this Behavior your target needs to provide two properties
//  - containsMouse: Defines if the mouse is inside the target
//  - animating: Defines if the target is being animated
// and one signal
//  - barrierTriggered: Defines when the pointer barrier has been triggered

AutoHideBehavior {
    id: intellihide

    property bool intelliHideShown: autoHideShown || (target !== undefined && !windows.intersects)

    shown: intelliHideShown

    WindowsIntersectMonitor {
        id: windows
        monitoredArea: {
            if (intellihide.target) {
                if (Utils.isLeftToRight()) {
                    return Qt.rect(declarativeView.screen.geometry.x,
                                   intellihide.target.y,
                                   intellihide.target.width,
                                   intellihide.target.height)
                } else {
                    return Qt.rect(declarativeView.screen.geometry.x + declarativeView.screen.availableGeometry.width - intellihide.target.width,
                                   intellihide.target.y,
                                   intellihide.target.width,
                                   intellihide.target.height)
                }
            } else {
                return Qt.rect(0, 0, 0, 0)
            }
        }
    }
}
