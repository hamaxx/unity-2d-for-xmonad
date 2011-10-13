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

            /* Tile considered 'visible' if it fully drawn */
            property int numberVisibleTiles: Math.floor(height / (tileSize + itemPadding))
            property int indexFirstVisibleTile: Math.ceil(contentY / (tileSize + itemPadding))
            property int indexLastVisibleTile: indexFirstVisibleTile + numberVisibleTiles - 1

            anchors.top: parent.top
            anchors.bottom: shelf.top
            anchors.bottomMargin: itemPadding
            width: parent.width

            autoScrollSize: tileSize / 2
            autoScrollVelocity: 200
            reorderable: true

            model: ListAggregatorModel {
                id: items
            }

            focus: true
            KeyNavigation.down: shelf
            KeyNavigation.up: shelf

            /* Implement wrapping and prevent shadow from overlapping a highlighted item */
            Keys.onPressed: {
                if (event.key == Qt.Key_Up) {
                    autoScrolling = true /* disable scroll animation as bounce effect problematic */
                    if (currentIndex == 0) {
                        shelf.currentIndex = shelf.count - 1
                        shelf.positionViewAtEnd()
                    } else if (currentIndex <= indexFirstVisibleTile + 1) {
                        positionViewAtIndex(currentIndex - 2, ListView.Beginning)
                    }
                } else if (event.key == Qt.Key_Down) {
                    autoScrolling = true
                    if (currentIndex == count - 1) {
                        shelf.currentIndex = 0
                        shelf.positionViewAtBeginning()
                    } else if (currentIndex >= indexLastVisibleTile - 1) {
                        positionViewAtIndex(currentIndex + 2, ListView.End)
                    }
                }
            }
            Keys.onReleased: {
                if (event.key == Qt.Key_Up || event.key == Qt.Key_Down) {
                    autoScrolling = false
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
            /* Ensure all delegates are cached in order to improve smoothness of
               scrolling on very low end platforms */
            cacheBuffer: 10000

            model: ListAggregatorModel {
                id: shelfItems
            }

            KeyNavigation.up: main
            KeyNavigation.down: main

            Keys.onPressed: {
                if (event.key == Qt.Key_Up && currentIndex == 0) {
                    main.currentIndex = main.count - 1
                    main.positionViewAtEnd()
                } else if (event.key == Qt.Key_Down && currentIndex == count - 1) {
                    main.currentIndex = 0
                    main.positionViewAtBeginning()
                }
            }
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
