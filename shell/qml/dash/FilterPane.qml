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
import Effects 1.0

FocusScope {
    id: filterPane

    property bool folded: true
    property int headerHeight
    property variant lens

    /* Give the focus to header when folded */
    onFoldedChanged: if (folded) header.focus = true

    AbstractButton {
        id: header

        Accessible.name: title.text

        effect: DropShadow {
                    blurRadius: {
                        switch (header.state) {
                            case "selected":
                            case "hovered":
                                return 4
                            case "pressed":
                                return 8
                            default:
                                return 0
                        }
                    }
                    offset.x: 0
                    offset.y: 0
                    color: "white"
                }

        onClicked: filterPane.folded = !filterPane.folded

        focus: true

        /* Do not navigate down to the options if they are folded */
        KeyNavigation.down: !filterPane.folded ? options : header

        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: parent.top
        height: headerHeight - anchors.topMargin

        TextCustom {
            id: title

            text: u2d.tr("Filter results")
            font.bold: true
            fontSize: "large"
            font.underline: parent.state == "selected"

            anchors.verticalCenter: parent.verticalCenter
            anchors.right: arrow.left
            anchors.rightMargin: 10
        }

        FoldingArrow {
            id: arrow

            folded: filterPane.folded

            anchors.verticalCenter: title.verticalCenter
            anchors.right: parent.right
        }
    }

    ListView {
        id: options

        clip: true
        opacity: folded ? 0.0 : 1.0
        Behavior on opacity {NumberAnimation {duration: 100; easing.type: Easing.InOutQuad}}

        KeyNavigation.up: header

        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: header.bottom
        anchors.topMargin: 7
        anchors.bottom: parent.bottom
        orientation: ListView.Vertical
        spacing: 12

        /* Make sure the first section is selected when getting the focus */
        currentIndex: 0
        onActiveFocusChanged: {
            /* It really should be onFocusChanged but the following bug makes
               using focus impossible (fixed in Qt 4.7.1):
               http://bugreports.qt.nokia.com/browse/QTBUG-12649
            */
            if(!focus) currentIndex = 0
        }

        /* Non-draggable when all items are visible */
        boundsBehavior: Flickable.StopAtBounds

        model: filterPane.lens != undefined ? filterPane.lens.filters : undefined
        /* Dynamically load the QML file corresponding to filter.rendererName.
           For example, if filter.rendererName == "filter-checkoption" then
           load "FilterCheckoption.qml".
        */
        delegate: FilterLoader {
            width: ListView.view.width
            lens: filterPane.lens
            filterModel: filter
            isFirst: index == 0
        }
    }
}
