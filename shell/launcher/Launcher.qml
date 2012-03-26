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
import Unity2d 1.0
import "../common"
import "../common/utils.js" as Utils

LauncherDropItem {
    id: launcher
    Accessible.name: "launcher"

    signal barrierTriggered

    property bool showMenus: true

    property bool containsMouse: declarativeView.monitoredAreaContainsMouse

    function hideMenu() {
        if (main.visibleMenu !== undefined) {
            main.visibleMenu.hide()
        }
        if (shelf.visibleMenu !== undefined) {
            shelf.visibleMenu.hide()
        }
    }

    function focusBFB() {
        if (!main.activeFocus) {
            main.focus = true
        }

        main.currentIndex = 0
        main.positionViewAtBeginning()
    }

    Item {
        anchors.fill: parent
        clip: true

        GnomeBackground {
            Accessible.name: "background"
            anchors.fill: parent
            overlay_color: "black"
            overlay_alpha: 0.66
            visible: !desktop.isCompositingManagerRunning
        }
    }

    /* The background color is the same as in the panel */
    Rectangle {
        Accessible.name: "background"
        anchors.fill: parent
        anchors.rightMargin: Utils.isLeftToRight() && !border.visible ? border.width : 0
        anchors.leftMargin:  Utils.isRightToLeft() && !border.visible ? border.width : 0
        color: Qt.darker(unity2dConfiguration.averageBgColor, 8.0)
        opacity: 0.66
        visible: desktop.isCompositingManagerRunning
    }

    Image {
        Accessible.name: "borderWithDash"
        id: border

        width: 1
        height: parent.height
        anchors.right: Utils.isLeftToRight() ? parent.right : undefined
        anchors.left:  Utils.isLeftToRight() ? undefined : parent.left
        visible: shellManager.dashActive && shellManager.dashShell == declarativeView
        source: "artwork/border.png"
        fillMode: Image.Stretch
    }

    Rectangle {
        Accessible.name: "borderWithoutDash"

        width: border.width
        height: parent.height
        anchors.right: Utils.isLeftToRight() ? border.anchors.right : undefined
        anchors.left:  Utils.isLeftToRight() ? undefined : border.anchors.left
        visible: !border.visible

        color: "white"
        opacity: 0.15
    }

    onDesktopFileDropped: applications.insertFavoriteApplication(path)
    onWebpageUrlDropped: applications.insertWebFavorite(url)

    FocusScope {
        Accessible.name: "content"

        focus: true
        anchors.fill: parent
        anchors.rightMargin: border.width
        z: 1 /* ensure the lists are always strictly on top of the background */

        LauncherList {
            id: main
            objectName: "main"

            /* function to position highlighted tile so that the shadow does not cover it */
            function positionMainViewForIndex(index) {
                /* Tile considered 'visible' if it fully drawn */
                var numberVisibleTiles = Math.floor(height / selectionOutlineSize)
                var indexFirstVisibleTile = Math.ceil(contentY / selectionOutlineSize)
                var indexLastVisibleTile = indexFirstVisibleTile + numberVisibleTiles - 1
                var nearestVisibleTile = Utils.clamp(index, indexFirstVisibleTile, indexLastVisibleTile)

                if (nearestVisibleTile == indexFirstVisibleTile) {
                    positionViewAtIndex(Math.max(index - 1, 0), ListView.Beginning)
                }
                else if (nearestVisibleTile == indexLastVisibleTile) {
                    positionViewAtIndex(Math.min(index + 1, count - 1), ListView.End)
                }
            }

            anchors.top: parent.top
            anchors.bottomMargin: 0
            /* the distance from the top of the launcher and the dash tile is 6 instead of 7 */
            anchors.topMargin: -1
            height: parent.height - shelf.height + ((selectionOutlineSize - tileSize)) - 4
            width: parent.width

            /* Ensure all delegates are cached in order to improve smoothness of
               scrolling on very low end platforms */
            cacheBuffer: 10000

            autoScrollSize: tileSize / 2
            autoScrollVelocity: 200
            reorderable: true
            showMenus: launcher.showMenus

            model: ListAggregatorModel {
                id: items
            }

            focus: true
            KeyNavigation.down: shelf

            /* Prevent shadow from overlapping a highlighted item */
            Keys.onPressed: {
                if (event.key == Qt.Key_Up) {
                    positionMainViewForIndex(currentIndex - 1)
                } else if (event.key == Qt.Key_Down) {
                    positionMainViewForIndex(currentIndex + 1)
                }
            }

            /* Always reset highlight to so-called BFB or Dash button */
            Connections {
                target: declarativeView
                onFocusChanged: {
                    if (declarativeView.focus && !main.flicking) {
                        hideMenu()
                    }
                }
            }
        }

        LauncherList {
            id: shelf
            objectName: "shelf"

            anchors.bottom: parent.bottom
            anchors.bottomMargin: main.anchors.bottomMargin
            anchors.topMargin: main.anchors.topMargin
            height: selectionOutlineSize * count
            width: parent.width
            interactive: false
            showMenus: launcher.showMenus

            model: ListAggregatorModel {
                id: shelfItems
            }

            KeyNavigation.up: main
        }
    }

    BfbModel {
        id: bfbModel
        dashManager: shellManager
    }

    ApplicationsList {
        id: applications
    }

    LauncherDevicesList {
        id: devices
    }

    WorkspacesList {
        id: workspaces
    }

    Trashes {
        id: trashes
    }

    Component.onCompleted: {
        items.appendModel(bfbModel);
        items.appendModel(applications);
        items.appendModel(workspaces);
        items.appendModel(devices);
        shelfItems.appendModel(trashes);
    }

    Connections {
        target: applications
        onApplicationBecameUrgent: {
            if (main.autoScrolling) {
                main.stopAutoScrolling()
            }

            /* index does not need to be translated because we know that
               applications are always first in the list. */
            main.positionViewAtIndex(index, ListView.Center)
        }
    }
}
