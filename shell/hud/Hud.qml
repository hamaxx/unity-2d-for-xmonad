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

    property alias searchQuery: searchEntry.searchQuery
    property bool active: false

    property int resultHeight: 42

    property bool animating: heightAnimation.running

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
            resultList.focus = true
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
        if (event.key == Qt.Key_Escape && searchEntry.searchQuery == "") toggleHud()
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
            width: 65

            Image {
                id: pip

                anchors.verticalCenter: iconTile.verticalCenter
                anchors.horizontalCenterOffset: -1
                anchors.left: parent.left
                mirror: Utils.isRightToLeft()

                source: "image://blended/%1color=%2alpha=%3"
                        .arg("launcher/artwork/launcher_arrow_ltr.png")
                        .arg("lightgrey").arg(1.0)
            }

            IconTile {
                id: iconTile

                anchors.top: parent.top
                anchors.topMargin: 5
                anchors.left: parent.left
                anchors.leftMargin: 5
                width: 54
                height: 54

                source: (resultList.currentItem != null && resultList.count > 0)
                        ? "image://icons/" + resultList.currentItem.icon
                        : (activeWindow.icon
                           ? "image://icons/" + activeWindow.icon
                           : "image://icons/unknown")
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
                onActivateFirstResult: executeResult(0)
            }

            ListView {
                id: resultList

                focus: true

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

                    onClicked: executeResult(resultId)
                }
            }
        }
    }
}
