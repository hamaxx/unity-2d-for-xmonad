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

/* The main component that manages the windows.
   This only acts as an outer shell, the actual logic is pretty much all in SpreadLayout.qml
   and SpreadItem.qml

   In the rest of the comments there will be some recurring terms that I explain below:
   - screen mode: in this mode each shot is positioned and scaled exactly as the real window.
   - spread mode: in this mode each shot is constrained to a cell in a layout.
   - intro animation: the animation that moves the shots from screen to spread mode
   - outro animation: the animation that moves the shots from spread mode back to screen mode

   The context property called control is the initiator of the entire spread process, and
   is triggered by D-Bus calls on the C++ side.

   The ScreenInfo's property availableGeometry represents the available space on the screen (i.e.
   screen minus launcher, panels, etc.).
*/

GridView {
    id: windows

    signal clicked
    signal windowActivated(variant window)

    MouseArea {
        anchors.fill: parent
        onClicked: windows.clicked()
        /* Eating all mouse events so that they are not passed beneath the workspace */
        hoverEnabled: true
    }

    /* This proxy model takes care of removing all windows that are not on
       the current workspace and that are not pinned to all workspaces. */
    SortFilterProxyModel {
        id: filteredByWorkspace
        model: switcher.allWindows
        dynamicSortFilter: true

        filterRole: WindowInfo.RoleWorkspace
        filterRegExp: RegExp("^%1|-2$".arg(workspace.workspaceNumber))
    }

    /* If there's any application filter set, this proxy model will remove
       all windows that do not belong to it. */
    SortFilterProxyModel {
        id: filteredByApplication
        model: filteredByWorkspace
        dynamicSortFilter: true

        filterRole: WindowInfo.RoleDesktopFile
        filterRegExp: RegExp("%1".arg(switcher.applicationFilter))
    }

    property int columns: Math.ceil(Math.sqrt(count))
    property int rows: Math.ceil(count / columns)
    property int cellSpacing: 10

    cellWidth: Math.floor(width / columns)
    cellHeight: height / rows

    model: filteredByApplication

    delegate:
        Item {
            id: cell

            ScaledItem {
                id: scaledCell

                target: cell
                itemWidth: window.size.width
                itemHeight: window.size.height
                parent: windows
            }

            /* Workaround http://bugreports.qt.nokia.com/browse/QTBUG-15642 where onAdd is not called for the first item */
            //GridView.onAdd:
            Component.onCompleted: if (!switcher.initial) addAnimation.start()
            GridView.onRemove: if (!switcher.initial) removeAnimation.start()

            width: windows.cellWidth
            height: windows.cellHeight

            Keys.onPressed: {
                switch (event.key) {
                    case Qt.Key_Enter:
                    case Qt.Key_Return:
                        windows.windowActivated(spreadWindow)
                        event.accepted = true
                }
            }

            Window {
                id: spreadWindow

                property bool animateFollow: false
                property bool followCell: true

                isSelected: cell.activeFocus

                onEntered: {
                    windows.currentIndex = index
                    cell.forceActiveFocus()
                }

                onClicked: windows.windowActivated(spreadWindow)

                /* Reparenting hack inspired by http://developer.qt.nokia.com/wiki/Drag_and_Drop_within_a_GridView */
                parent: windows

                z: window.z

                /* Duplicated animation code because QML does not support grouping
                   of identical Behaviors yet.
                   http://bugreports.qt.nokia.com/browse/QTBUG-16375
                */
                Behavior on x { enabled: spreadWindow.animateFollow; NumberAnimation { duration: Utils.transitionDuration; easing.type: Easing.InOutQuad } }
                Behavior on y { enabled: spreadWindow.animateFollow; NumberAnimation { duration: Utils.transitionDuration; easing.type: Easing.InOutQuad } }
                Behavior on width { enabled: spreadWindow.animateFollow; NumberAnimation { duration: Utils.transitionDuration; easing.type: Easing.InOutQuad } }
                Behavior on height { enabled: spreadWindow.animateFollow; NumberAnimation { duration: Utils.transitionDuration; easing.type: Easing.InOutQuad } }

                windowInfo: window
                state: windows.state
                states: [
                    State {
                        name: "screen"
                        PropertyChanges {
                            target: spreadWindow
                            /* Note that we subtract the availableGeometry x and y since window.location is
                            expressed in global screen coordinates. */
                            x: window.position.x - declarativeView.globalPosition.x
                            y: window.position.y - declarativeView.globalPosition.y
                            width: window.size.width
                            height: window.size.height
                            animateFollow: false
                        }
                    },
                    State {
                        name: "spread"
                        PropertyChanges {
                            target: spreadWindow
                            /* Center the window in its cell */
                            x: followCell ? scaledCell.x + cellSpacing : x
                            y: followCell ? scaledCell.y + cellSpacing : y
                            width: followCell ? scaledCell.width - cellSpacing * 2 : width
                            height: followCell ? scaledCell.height - cellSpacing * 2: height
                            animateFollow: !switcher.initial
                        }
                    }
                ]

                SequentialAnimation {
                    id: addAnimation

                    PropertyAction { target: spreadWindow; property: "animateFollow"; value: false }
                    NumberAnimation { target: spreadWindow; property: "opacity"; from: 0; to: 1.0; duration: Utils.transitionDuration; easing.type: Easing.InOutQuad }
                    PropertyAction { target: spreadWindow; property: "animateFollow"; value: true }
                }
                SequentialAnimation {
                    id: removeAnimation

                    /* FIXME: do not work if windowInfo is destroyed */
                    PropertyAction { target: spreadWindow; property: "followCell"; value: false }
                    NumberAnimation { target: spreadWindow; property: "opacity"; to: 0.0; duration: Utils.transitionDuration; easing.type: Easing.InOutQuad }
                    /* spreadWindow was reparented from cell and will not be deleted when cell is.
                       Delete it manually. */
                    ScriptAction { script: spreadWindow.deleteLater() }
                }
                transitions: [
                    Transition {
                        to: "screen,spread"
                        SequentialAnimation {
                            PropertyAction { target: spreadWindow; property: "animating"; value: true }
                            NumberAnimation {
                                properties: "x,y,width,height"
                                duration: Utils.transitionDuration
                                easing.type: Easing.InOutQuad
                            }
                            /* Apply final value to spreadWindow.animateFollow by not specifying a value */
                            PropertyAction { property: "animateFollow" }
                            PropertyAction { target: spreadWindow; property: "animating"; value: false }
                        }
                    }
                ]
            }
    }
}
