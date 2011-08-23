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
    id: searchRefine

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

        onClicked: searchRefine.folded = !searchRefine.folded

        focus: true

        /* Do not navigate down to the options if they are folded */
        KeyNavigation.down: !searchRefine.folded ? options : header

        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.topMargin: 10
        height: headerHeight - anchors.topMargin

        TextCustom {
            id: title

            text: u2d.tr("Refine search")
            font.bold: true
            font.pixelSize: 16
            font.underline: parent.state == "selected"

            anchors.top: parent.top
            anchors.left: parent.left
        }

        FoldingArrow {
            id: arrow

            folded: searchRefine.folded

            anchors.verticalCenter: title.verticalCenter
            anchors.left: title.right
            anchors.leftMargin: 10
        }
    }

    ListView {
        id: options

        opacity: folded ? 0.0 : 1.0
        Behavior on opacity {NumberAnimation {duration: 100; easing.type: Easing.InOutQuad}}

        KeyNavigation.up: header

        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: header.bottom
        anchors.topMargin: 7
        anchors.bottom: parent.bottom
        orientation: ListView.Vertical

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

        model: searchRefine.lens.filters
        /* Use a Loader to dynamically load the right QML depending on filter.rendererName

           CheckOptionFilter: filter-checkoption
           MultiRangeFilter: filter-multirange     TODO
           RadioOptionFilter: filter-radiooption
           RatingsFilter: filter-ratings
        */
        delegate: Loader {
            anchors.left: parent.left
            anchors.right: parent.right

            focus: true

            property string title: u2d.tr(filter.name)
            property variant lens: searchRefine.lens
            property variant filterModel: filter
            source: dash.convertToCamelCase(filter.rendererName) + ".qml"

            onLoaded: {
                item.title = title
                item.lens = lens
                item.filterModel = filterModel
            }

            /* FIXME: add an "all" button
               filter.filtering is a bool indicating its state
               filter.clear() is the method that should be used when clicking on it */
        }
    }
}
