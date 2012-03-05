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

// Shows the target when it has the focus or when you move the
// mouse for 500 msec to the edge of the target
// Hides the target when none of the above conditions are met
// and you have not had the mouse over it during more than 1000 msec
// To use this Behavior your target needs to provide two properties
//  - containsMouse: Defines if the mouse is inside the target
//  - outerEdgeContainsMouse: Defines if the mouse is in the edge of the target

BaseBehavior {
    id: autoHide

    property bool shownRegardlessOfFocus: true
    property bool autoHideShown: target !== undefined && (target.activeFocus || shownRegardlessOfFocus)

    shown: autoHideShown

    onForcedVisibleChanged:
    {
        if (!forcedVisible) {
            if (!target.containsMouse && forcedVisibleChangeId != "dash") {
                shownRegardlessOfFocus = true
                autoHideTimer.restart()
            }
        }
    }

    Timer {
        id: autoHideTimer
        interval: 1000
        running: (target !== undefined) ? !target.containsMouse : false
        onTriggered: shownRegardlessOfFocus = false
    }

    Timer {
        id: edgeHitTimer
        interval: 500
        onTriggered: shownRegardlessOfFocus = true
    }

    Connections {
        target: (autoHide.target !== undefined) ? autoHide.target : null
        onContainsMouseChanged: {
            if ((shown || forcedVisible) && target.containsMouse) {
                shownRegardlessOfFocus = true
            }
        }
    }

    Connections {
        target: autoHide.target !== undefined ? autoHide.target : null
        onOuterEdgeContainsMouseChanged: edgeHitTimer.running = target.outerEdgeContainsMouse
        ignoreUnknownSignals: true
    }
}
