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
import "../common"
import "../common/utils.js" as Utils

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
        id: edgeHitTimer
        interval: 500
        onTriggered: shownBecauseOfMousePosition = true
    }

    Timer {
        id: mouseLeaveTimer
        interval: 1000
        onTriggered: shownBecauseOfMousePosition = false
    }

    Connections {
        target: (intellihide.target !== undefined) ? intellihide.target : null
        onOuterEdgeContainsMouseChanged: edgeHitTimer.running = target.outerEdgeContainsMouse
        ignoreUnknownSignals: true
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
