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

// Shows the target when it has the focus or when you move the
// mouse for 500 msec to the edge of the target or there are no 
// windows that intersect with the target
// Hides the target when none of the above conditions are met
// and you have not had the mouse over it during more than 1000 msec
// To use this Behavior your target needs to provide one properties
//  - containsMouse: Defines if the mouse is inside the target
// and one signal
//  - barrierBroken: Defines when the pointer barrier has been broken

BaseBehavior {
    id: intellihide

    property bool shownBecauseOfMousePosition: false

    shown: target !== undefined && (target.activeFocus || shownBecauseOfMousePosition || !windows.intersects)

    onForcedVisibleChanged:
    {
        if (!forcedVisible) {
            if (!target.containsMouse && forcedVisibleChangeId != "dash") {
                shownBecauseOfMousePosition = true
                mouseLeaveTimer.restart()
            }
        }
    }

    Timer {
        id: mouseLeaveTimer
        interval: 1000
        onTriggered: shownBecauseOfMousePosition = false
    }

    Connections {
        target: (intellihide.target !== undefined) ? intellihide.target : null
        onBarrierBroken: shownRegardlessOfFocus = true
    }

    Connections {
        target: (intellihide.target !== undefined) ? intellihide.target : null
        onContainsMouseChanged: {
            if ((shown || forcedVisible) && target.containsMouse) {
                shownBecauseOfMousePosition = true
            }
            mouseLeaveTimer.running = !target.containsMouse
        }
        ignoreUnknownSignals: true
    }

    WindowsIntersectMonitor {
        id: windows
        monitoredArea: {
            if (intellihide.target) {
                if (Utils.isLeftToRight()) {
                    return Qt.rect(0,
                                   intellihide.target.y,
                                   intellihide.target.width,
                                   intellihide.target.height)
                } else {
                    return Qt.rect(declarativeView.screen.availableGeometry.width - intellihide.target.width,
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
