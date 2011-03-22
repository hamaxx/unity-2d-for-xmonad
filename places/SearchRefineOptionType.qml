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

import Qt 4.7

SearchRefineOption {
    id: searchRefineOption

    AbstractButton {
        id: header

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
        }
    }

    GridView {
        id: filters

        property int columns: 2
        property int rowsPerColumn: Math.ceil(count/columns)

        cellHeight: 35
        cellWidth: width/columns

        anchors.top: header.bottom
        anchors.topMargin: 15
        height: cellHeight * rowsPerColumn
        boundsBehavior: Flickable.StopAtBounds

        anchors.left: parent.left
        anchors.right: parent.right

        flow: GridView.TopToBottom

        /* Make sure the first item is selected when getting the focus for the first time */
        currentIndex: 0

        delegate: TickBox {
            height: filters.cellHeight
            width: filters.cellWidth-10
            /* Not checking for placeEntryModel != undefined leads to a segfault
               when switching places */
            text: placeEntryModel != undefined ? column_0 : ""
            ticked: dash.currentPage.model.activeSection == model.index

            onClicked: placeEntryModel.activeSection = model.index
        }

        model: placeEntryModel != undefined ? placeEntryModel.sections : undefined
    }
}
