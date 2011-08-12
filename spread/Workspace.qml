/*
 * This file is part of unity-2d
 *
 * Copyright 2010-2011 Canonical Ltd.
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
import Unity2d 1.0

FocusScope {
    id: workspace

    transformOrigin: Item.TopLeft

    property real unzoomedScale
    property int unzoomedX
    property int unzoomedY
    property real zoomedScale
    property int zoomedX
    property int zoomedY

    signal clicked

    GnomeBackground {
        anchors.fill: parent
        overlay_color: "black"
        overlay_alpha: 0

        clip: true
        cached: false
    }

    Windows {
        state: workspace.state == "screen" ? "screen" : "spread"
        anchors.fill: parent
        focus: true
        onClicked: workspace.clicked()
        onWindowActivated: {
            if (workspace.state != "zoomed") {
                workspace.clicked()
            } else {
                /* Hack to make sure the window is on top of the others during the
                   outro animation */
                window.z = 9999
                switcher.activateWindow(window.windowInfo)
            }
        }
    }

    states: [
        State {
            name: "unzoomed"
            PropertyChanges {
                target: workspace
                scale: unzoomedScale
                x: unzoomedX
                y: unzoomedY
                z: 0
            }
        },
        State {
            name: "zoomed"
            PropertyChanges {
                target: workspace
                scale: zoomedScale
                x: zoomedX
                y: zoomedY
                z: 2
            }
        },
        State {
            name: "screen"
            PropertyChanges {
                target: workspace
                scale: 1.0
                x: 0
                y: 0
                z: 2
            }
        }
    ]

    transitions: [
        Transition {
            NumberAnimation {
                target: workspace
                properties: "x,y,scale"
                duration: Utils.transitionDuration
                easing.type: Easing.InOutQuad
            }
        },
        Transition {
            to: "unzoomed"
            SequentialAnimation {
                /* When going to default state put the workspace underneath the
                   workspace in zoomed state but not on the same plane as the
                   workspaces also in the default state until the end of the transition. */
                PropertyAction { property: "z"; value: 1 }
                NumberAnimation {
                    target: workspace
                    properties: "x,y,scale"
                    duration: Utils.transitionDuration
                    easing.type: Easing.InOutQuad
                }
                PropertyAction { property: "z" }
            }
        }
    ]
}
