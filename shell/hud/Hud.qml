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

    property variant currentPage
    /* FIXME: 'active' property exactly mirrors 'declarativeView.hudActive'.
       The final goal is to transition to using exclusively the QML 'active' property
       and drop the C++ 'declarativeView.hudActive'.
    */
    property variant active
    /* The following way of mirroring the values of 'declarativeView.hudActive'
       and 'active' works now and QML does not see it as a binding loop but we
       cannot count on it long term.
    */
    property int queryHeight: 42
    property int listLowerMargin: 10

    property bool animating: heightAnimation.running
    height: container.height + layout.anchors.bottomMargin + container.anchors.topMargin + listLowerMargin
    Behavior on height { PropertyAnimation { id: heightAnimation; duration: 150; easing.type: Easing.InOutQuad } }

    Binding {
        target: declarativeView
        property: "hudActive"
        value: hud.active
    }
    Binding {
        target: hud
        property: "active"
        value: declarativeView.hudActive
    }

    onActiveChanged: {
        if (active) {
            declarativeView.forceActivateWindow()
            hudModel.searchText = "" /* helps ensure HUD ready */
            searchEntry.focus = true
        }
        else {
            hudModel.endSearch
            resultList.currentIndex = 0
        }
    }

    property variant hudModel: Hud {}

    Background {
        id: background

        anchors.fill: parent

        active: hud.active
    }

    Item {
        id: layout

        anchors.fill: parent
        /* Margins in DesktopMode set so that the content does not overlap with
           the border defined by the background image.
        */
        anchors.bottomMargin: 39
        anchors.rightMargin: 37

        clip: true

        Item {
            id: tile

            anchors.left: parent.left
            anchors.top: parent.top
            anchors.bottom: parent.bottom
            width: 65

            visible: resultList.activeFocus

            Image {
                id: pip
                anchors.verticalCenter: parent.verticalCenter
                anchors.left: parent.left
                mirror: Utils.isRightToLeft()

                source: "image://blended/%1color=%2alpha=%3"
                        .arg("launcher/artwork/launcher_arrow_ltr.png")
                        .arg("lightgrey").arg(1.0)
            }

            IconTile {
                anchors.top: parent.top
                anchors.topMargin: 5
                anchors.left: parent.left
                anchors.leftMargin: 6
                width: 54
                height: 54
                source: (resultList.currentItem != null)
                        ? "image://icons/" + resultList.currentItem.icon
                        : "image://icons/unknown"
            }
        }

        /* Unhandled keys will always be forwarded to the search bar. That way
           the user can type and search from anywhere in the interface without
           necessarily focusing the search bar first. */
        Keys.forwardTo: [searchEntry]

        Rectangle{
            id: container
            anchors.top: parent.top
            anchors.topMargin: 11
            anchors.left: tile.right
            anchors.leftMargin: 10
            anchors.right: parent.right
            anchors.rightMargin: 10
            height: resultList.count * resultHeight + searchEntry.height
            border.color: "#21ffffff" // 80% opaque
            border.width: 1
            color: "transparent"
            radius: 7
            smooth: true

            SearchEntry {
                id: searchEntry

                focus: true
                KeyNavigation.down: (resultList.count > 0 ) ? resultList : null

                anchors.top: parent.top
                anchors.left: parent.left
                anchors.right: parent.right

                height: 42
                opacity: 1

                active: hud.active
                placeHolderText: u2d.tr("Type your Command")

                onSearchQueryChanged: hudModel.searchQuery = searchQuery
                onActivateFirstResult: {
                    hudModel.executeResult(0)
                    hud.active = false
                }
            }

            ListView {
                id: resultList

                Accessible.name: "result list"
                KeyNavigation.up: searchEntry

                model: hudModel

                anchors.top: searchEntry.bottom
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.bottom: parent.bottom

                boundsBehavior: ListView.StopAtBounds

                delegate: ResultItem{
                    height: resultHeight
                    width: ListView.view.width

                    icon: iconName /* expose this property for tile */

                    onClicked: {
                        hudModel.executeResult(resultId)
                        hud.active = false
                    }
                }
            }
        }
    }
}
