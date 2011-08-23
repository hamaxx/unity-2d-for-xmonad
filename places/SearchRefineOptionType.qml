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

    GridViewWithSpacing {
        id: filters

        columns: 2
        rows: Math.ceil(count/columns)

        horizontalSpacing: 10
        verticalSpacing: 12

        cellWidth: width/columns
        delegateHeight: 30

        anchors.top: header.bottom
        anchors.topMargin: 15
        height: cellHeight * rows
        boundsBehavior: Flickable.StopAtBounds

        anchors.left: parent.left
        anchors.right: parent.right

        /* Make sure the first item is selected when getting the focus for the first time */
        currentIndex: 0
        KeyNavigation.up: header

        delegate: FocusScope {
            TickBox {
                focus: true
                width: filters.delegateWidth
                height: filters.delegateHeight
                x: filters.delegateX(model.index % filters.columns)
                       /* Not checking for lens != undefined leads to a segfault
                          when switching lenses */
                       text: lens != undefined ? column_0 : ""
                       checked: dash.currentPage.model.activeSection == model.index

                       onClicked: lens.activeSection = model.index
            }
        }

        model: lens != undefined ? lens.sections : undefined
    }
}
