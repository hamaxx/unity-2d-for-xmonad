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

// Shows the target when it has the focus or when you trigger the pointer barrier
// in the edge of the target
// Hides the target when none of the above conditions are met
// and you have not had the mouse over it during more than 1000 msec
// To use this Behavior your target needs to provide one properties
//  - containsMouse: Defines if the mouse is inside the target
// and one signal
//  - barrierTriggered: Defines when the pointer barrier has been triggered

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

    onForcedHiddenChanged:
    {
        if (forcedHidden) {
            autoHideTimer.stop()
            if (!target.containsMouse) {
                shownRegardlessOfFocus = false
            }
        }
    }

    Timer {
        id: autoHideTimer
        interval: 1000
        running: (target !== undefined) ? !target.containsMouse : false
        onTriggered: shownRegardlessOfFocus = false
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
        onBarrierTriggered: shownRegardlessOfFocus = true
        onAnimatingChanged:
        {
            if (!target.animating && !target.containsMouse && shownRegardlessOfFocus) {
                autoHideTimer.start();
            }
        }
    }
}
