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
import "utils.js" as Utils

Item {
    id: controller
    property bool shown: true
    property variant behavior: null
    property bool forceVisible: false
    property bool forceHidden: false
    property variant forceVisibleChangeId
    property variant forceVisibleStack: {}
    property variant forceHiddenStack: {}

    Binding {
        target: controller
        property: "shown"
        value: (!forceHidden) ? forceVisible || behavior.shown : false
        when: behavior != null
    }

    function beginForceVisible(id) {
        /* We need to copy the property to a JS variable before we can properly interact
           with it and then copy it back to the QML property when done. This seems to be
           a limitation of QML that will be fixed in QtQuick 2.0. For more information:
           https://bugreports.qt.nokia.com/browse/QTBUG-21842
        */
        var stack = controller.forceVisibleStack
        var wasEmpty = Utils.hashEmpty(stack)

        if (forceHidden) console.log("DEBUG: beginForceVisible for id \"" + id +
                        "\" called when forceHidden still true")

        if (stack[id]) stack[id] += 1
        else stack[id] = 1
        controller.forceVisibleStack = stack

        if (wasEmpty) {
            forceVisibleChangeId = id
            forceVisible = true
        }
    }

    function endForceVisible(id) {
        var stack = controller.forceVisibleStack

        if (stack[id]) {
            stack[id] -= 1
            if (stack[id] === 0) delete stack[id]
        } else console.log("DEBUG: endForceVisible for id \"" + id +
                           "\" called without matching startForceVisible")

        controller.forceVisibleStack = stack
        if (Utils.hashEmpty(stack)) {
            forceVisibleChangeId = id
            forceVisible = false
        }
    }

    function beginForceHidden(id) {
        var stack = controller.forceHiddenStack
        var wasEmpty = Utils.hashEmpty(stack)

        if (forceVisible) console.log("DEBUG: beginForceHidden for id \"" + id +
                        "\" called when forceVisible still true")

        if (stack[id]) stack[id] += 1
        else stack[id] = 1
        controller.forceHiddenStack = stack

        if (wasEmpty) {
            forceVisibleChangeId = id
            forceHidden = true
        }
    }

    function endForceHidden(id) {
        var stack = controller.forceHiddenStack

        if (stack[id]) {
            stack[id] -= 1
            if (stack[id] === 0) delete stack[id]
        } else console.log("DEBUG: endForceHidden for id \"" + id +
                           "\" called without matching startForceHidden")

        controller.forceHiddenStack = stack
        if (Utils.hashEmpty(stack)) {
            forceVisibleChangeId = id
            forceHidden = false
        }
    }
}
