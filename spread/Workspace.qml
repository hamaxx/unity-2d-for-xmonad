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

    property real unzoomedScale
    property real zoomedScale
    property int zoomedX
    property int zoomedY
    property alias windowCount: windows.count

    property int screenOriginX
    property int screenOriginY
    signal clicked
    signal entered

    onStateChanged: {
        screenOriginX = mapFromItem(switcher, 0, 0).x
        screenOriginY = mapFromItem(switcher, 0, 0).y
    }

    Item {
        id: workspaceWindow

        transformOrigin: Item.TopLeft
        width: switcher.width
        height: switcher.height
        state: parent.state

        GnomeBackground {
            anchors.fill: parent
            overlay_color: "black"
            overlay_alpha: 0

            clip: true
            cached: false
            offsetX: -declarativeView.screen.panelsFreeGeometry.x + declarativeView.screen.geometry.x
            offsetY: -declarativeView.screen.panelsFreeGeometry.y + declarativeView.screen.geometry.y
        }

        Windows {
            id: windows
            state: workspace.state
            anchors.fill: parent
            focus: true
            onClicked: workspace.clicked()
            onEntered: workspace.entered()
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
                    target: workspaceWindow
                    scale: unzoomedScale
                    x: switcher.spacing / 2
                    y: switcher.spacing / 2
                }
                PropertyChanges {
                    target: workspace
                    z: 0
                }
            },
            State {
                name: "zoomed"
                PropertyChanges {
                    target: workspaceWindow
                    scale: zoomedScale
                    x: zoomedX + workspace.screenOriginX
                    y: zoomedY + workspace.screenOriginY
                }
                PropertyChanges {
                    target: workspace
                    z: 2
                }
            },
            State {
                name: "screen"
                PropertyChanges {
                    target: workspaceWindow
                    scale: 1.0
                    x: workspace.screenOriginX
                    y: workspace.screenOriginY
                }
                PropertyChanges {
                    target: workspace
                    z: 2
                }
            }
        ]

        transitions: [
            Transition {
                /* Disable animations when coming from the base state */
                from: ""
            },
            Transition {
                NumberAnimation {
                    target: workspaceWindow
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
                    PropertyAction { target: workspace; property: "z"; value: 1 }
                    NumberAnimation {
                        target: workspaceWindow
                        properties: "x,y,scale"
                        duration: Utils.transitionDuration
                        easing.type: Easing.InOutQuad
                    }
                    PropertyAction { target: workspace; property: "z" }
                }
            }
        ]
    }

    Keys.onPressed: {
        switch (event.key) {
            case Qt.Key_Enter:
            case Qt.Key_Return:
            {
                clicked()
                event.accepted = true
            }
        }
    }

    function setFocusOnFirstWindow() {
        windows.currentIndex = 0
    }

    function setFocusOnLastWindow() {
        windows.currentIndex = windows.count - 1
    }
}
