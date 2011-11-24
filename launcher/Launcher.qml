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
import Unity2d 1.0 /* required for drag’n’drop handling */

LauncherDropItem {
    id: launcher

    Accessible.name: "root"

    function clamp(x, min, max) {
        return Math.max(Math.min(x, max), min)
    }

    function hideMenu() {
        if (main.visibleMenu !== undefined) {
            main.visibleMenu.hide()
        }
        if (shelf.visibleMenu !== undefined) {
            shelf.visibleMenu.hide()
        }
    }

    function positionAtBeginning() {
        main.positionViewAtBeginning()
        hideMenu()
    }

    GnomeBackground {
        Accessible.name: "background"
        anchors.fill: parent
        overlay_color: "black"
        overlay_alpha: 0.66
        visible: !screen.isCompositingManagerRunning
    }

    Rectangle {
        Accessible.name: "background"
        anchors.fill: parent
        color: "black"
        opacity: 0.66
        visible: screen.isCompositingManagerRunning
    }
    
    Image {
        Accessible.name: "border"
        id: border

        width: 1
        height: parent.height
        anchors.right: parent.right
        fillMode: Image.TileVertically
        source: "artwork/background.png"
    }

    onDesktopFileDropped: applications.insertFavoriteApplication(path)
    onWebpageUrlDropped: applications.insertWebFavorite(url)

    FocusScope {
        Accessible.name: "content"

        focus: true
        anchors.fill: parent
        z: 1 /* ensure the lists are always strictly on top of the background */

        LauncherList {
            id: main
            Accessible.name: "main"

            /* function to position highlighted tile so that the shadow does not cover it */
            function positionMainViewForIndex(index) {
                /* Tile considered 'visible' if it fully drawn */
                var numberVisibleTiles = Math.floor(height / (tileSize + itemPadding))
                var indexFirstVisibleTile = Math.ceil(contentY / (tileSize + itemPadding))
                var indexLastVisibleTile = indexFirstVisibleTile + numberVisibleTiles - 1
                var nearestVisibleTile = clamp(index, indexFirstVisibleTile, indexLastVisibleTile)

                if (nearestVisibleTile == indexFirstVisibleTile) {
                    positionViewAtIndex(Math.max(index - 1, 0), ListView.Beginning)
                }
                else if (nearestVisibleTile == indexLastVisibleTile) {
                    positionViewAtIndex(Math.min(index + 1, count - 1), ListView.End)
                }
            }

            anchors.top: parent.top
            anchors.bottom: shelf.top
            anchors.bottomMargin: itemPadding
            width: parent.width

            /* Ensure all delegates are cached in order to improve smoothness of
               scrolling on very low end platforms */
            cacheBuffer: 10000

            autoScrollSize: tileSize / 2
            autoScrollVelocity: 200
            reorderable: true

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
                target: launcherView
                onFocusChanged: {
                    if (launcherView.focus && !main.flicking) {
                        hideMenu()
                    }
                }
            }
        }

        LauncherList {
            id: shelf
            Accessible.name: "shelf"

            anchors.bottom: parent.bottom
            anchors.bottomMargin: main.anchors.bottomMargin
            height: (tileSize + itemPadding) * count
            width: parent.width
            itemPadding: 0
            interactive: false

            model: ListAggregatorModel {
                id: shelfItems
            }

            KeyNavigation.up: main
        }
    }

    BfbModel {
        id: bfbModel
    }

    LauncherApplicationsList {
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

    Keys.onPressed: {
        if( event.key == Qt.Key_Escape ){
            launcherView.forceDeactivateWindow()
            event.accepted = true
        }
    }

    Component.onCompleted: {
        items.appendModel(bfbModel);
        items.appendModel(applications);
        items.appendModel(workspaces);
        items.appendModel(devices);
        shelfItems.appendModel(trashes);
    }

    Connections {
        target: launcherView
        onAddWebFavoriteRequested: applications.insertWebFavorite(url)
        onSuperKeyHeldChanged: {
            if (superKeyHeld) visibilityController.beginForceVisible()
            else visibilityController.endForceVisible()
        }
        onFocusChanged: {
            if (focus) visibilityController.beginForceVisible()
            else visibilityController.endForceVisible()
        }
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
