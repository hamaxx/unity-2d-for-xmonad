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

SearchRefineOption {
    id: searchRefineOption

    Item {
        id: header

        Accessible.name: searchRefineOption.title

        KeyNavigation.down: filters

        focus: true
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        height: childrenRect.height

        TextCustom {
            id: title

            anchors.top: parent.top
            anchors.left: parent.left

            text: searchRefineOption.title
            font.pixelSize: 16
            font.bold: true
            font.underline: ( parent.state == "selected" || parent.state == "hovered" )
        }
    }

    GridView {
        id: filters

        property int columns: 2
        property int rowsPerColumn: Math.ceil(count/columns)

        cellHeight: 42 //30px for button + 12px for vertical padding
        cellWidth: width/columns //145px for button + 10px for horizonal padding

        anchors.top: header.bottom
        anchors.topMargin: 15
        height: cellHeight * rowsPerColumn
        boundsBehavior: Flickable.StopAtBounds

        anchors.left: parent.left
        anchors.right: parent.right

        flow: GridView.TopToBottom

        /* Make sure the first item is selected when getting the focus for the first time */
        currentIndex: 0
        KeyNavigation.up: header

        delegate: TickBox {
            height: filters.cellHeight-13 //29 = filters.cellHeight - vertical padding (10) - fix (3)
            width: filters.cellWidth-13 //144 = filters.cellWidth - horizontal padding (12) - fix (1)
            /* Not checking for lens != undefined leads to a segfault
               when switching lenses */
            text: lens != undefined ? column_0 : ""
            checked: dash.currentPage.model.activeSection == model.index

            onClicked: lens.activeSection = model.index
        }

        model: lens != undefined ? lens.sections : undefined
    }
}
