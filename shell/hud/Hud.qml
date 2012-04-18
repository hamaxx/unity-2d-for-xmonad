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

import QtQuick 1.1
import Unity2d 1.0
import Effects 1.0
import "../common"
import "../common/utils.js" as Utils

FocusScope {
    id: hud
    objectName: "Hud"
    Accessible.name: "Hud"

    LayoutMirroring.enabled: Utils.isRightToLeft()
    LayoutMirroring.childrenInherit: true

    property bool active: false

    property int resultHeight: 42

    property bool animating: heightAnimation.running
    property string appIcon: ""

    height: layout.childrenRect.height + layout.anchors.bottomMargin + 10

    Behavior on height {
        enabled: desktop.isCompositingManagerRunning
        PropertyAnimation { id: heightAnimation; duration: 150; easing.type: Easing.InOutQuad }
    }

    WindowInfo {
        id: activeWindow
        contentXid: shellManager.lastFocusedWindow
    }

    SpreadMonitor {
        id: spread
        onShownChanged: active = false
    }

    onActiveChanged: {
        if (active) {
            shellManager.hudShell.forceActivateWindow()
            appIcon = getActiveWindowIcon()
        } else {
            hudModel.endSearch()
            resultList.currentIndex = -1
        }
    }

    Connections {
        target: shellManager

        onToggleHud: toggleHud()
        onHudShellChanged: {
            if (active) {
                background.trigger()
            }
        }
    }

    Keys.onPressed: {
        if (event.key == Qt.Key_Escape) toggleHud()
        else if (event.key == Qt.Key_Down) {
            resultList.incrementCurrentIndex()
            resultList.forceActiveFocus();
        }
        else if (event.key == Qt.Key_Up) {
            resultList.decrementCurrentIndex()
            resultList.forceActiveFocus();
        }
    }

    function toggleHud() {
        if (spread.shown) return
        if (active) shellManager.hudShell.forceDeactivateWindow()
        active = !active
    }

    function executeResult(resultId) {
        shellManager.hudShell.forceDeactivateWindow()
        hudModel.executeResult(resultId)
        active = false
    }

    function getActiveWindowIcon() {
        return activeWindow.icon ? "image://icons/" + activeWindow.icon
                                 : "image://icons/unknown"
    }

    property variant hudModel: Hud {}

    Background {
        id: background

        anchors.fill: parent

        active: hud.active
        view: shellManager.hudShell
    }

    Item {
        id: layout

        anchors.fill: parent

        /* Margins so content does not overlap with the background border */
        anchors.bottomMargin: background.bottomBorderThickness
        anchors.rightMargin: background.rightBorderThickness

        clip: true

        Image {
            id: panelBorder

            height: 1
            anchors.top: parent.top
            anchors.left: parent.left
            anchors.leftMargin: tile.width
            anchors.right: parent.right
            source: "../common/artwork/panel_border.png"
            fillMode: Image.Stretch
        }

        Item {
            id: tile

            anchors.left: parent.left
            anchors.top: parent.top
            width: visible ? launcherLoader.width : 0
            visible: !shellManager.hudShell.rootObject.launcherAlwaysVisible

            IconTile {
                id: iconTile

                anchors.top: parent.top
                anchors.topMargin: 5
                anchors.left: parent.left
                anchors.leftMargin: 5
                width: 54
                height: 54

                source: appIcon

                tileBackgroundImage: "../launcher/artwork/squircle_base_54.png"
                tileShineImage: "../launcher/artwork/squircle_shine_54.png"
                selectedTileBackgroundImage: "../launcher/artwork/squircle_base_selected_54.png"
            }

            Image {
                id: pip

                anchors.verticalCenter: iconTile.verticalCenter
                anchors.right: tile.right
                mirror: Utils.isRightToLeft()

                source: "image://blended/%1color=%2alpha=%3"
                        .arg("launcher/artwork/launcher_arrow_rtl.png")
                        .arg("lightgrey").arg(1.0)
            }
        }

        /* Unhandled keys will always be forwarded to the search bar. That way
           the user can type and search from anywhere in the interface without
           necessarily focusing the search bar first. */
        Keys.forwardTo: [searchEntry]

        Rectangle {
            id: container

            anchors.top: parent.top
            anchors.topMargin: 11
            anchors.left: tile.right
            anchors.leftMargin: 10
            anchors.right: parent.right
            anchors.rightMargin: 10
            height: childrenRect.height

            border.color: "#21ffffff" // 80% opaque
            border.width: 1
            color: "transparent"
            radius: 7
            smooth: true

            SearchEntry {
                id: searchEntry
                focus: true

                anchors.top: parent.top
                anchors.left: parent.left
                anchors.right: parent.right
                height: 42

                opacity: 1

                active: hudModel.connected && hud.active
                placeHolderText: (hudModel.connected) ? u2d.tr("Type your Command")
                                                      : u2d.tr("Error: HUD service not connected")

                onSearchQueryChanged: {
                    hudModel.searchQuery = searchQuery
                    resultList.currentIndex = 0
                }
                onActivateFirstResult: executeResult(resultList.currentIndex)
            }

            ListView {
                id: resultList

                focus: false

                Accessible.name: "result list"

                model: hudModel

                anchors.top: searchEntry.bottom
                anchors.left: parent.left
                anchors.right: parent.right
                height: count * resultHeight

                boundsBehavior: ListView.StopAtBounds

                delegate: ResultItem {
                    height: resultHeight
                    width: ListView.view.width

                    icon: iconName /* expose this property for tile */

                    current: ListView.isCurrentItem

                    onClicked: executeResult(resultId)
                    onMouseOverChanged: {
                        if (mouseOver) {
                            resultList.currentIndex = model.index;
                        }
                    }
                }

                onCurrentItemChanged: {
                    if (currentItem != null && count > 0) {
                        appIcon = "image://icons/" + (currentItem.icon ? currentItem.icon : "unknown")
                    } else {
                        appIcon = getActiveWindowIcon()
                    }
                }
            }
        }
    }
}
