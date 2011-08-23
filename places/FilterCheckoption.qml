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

Filter {
    id: filterView

    Item {
        id: header

        Accessible.name: filterView.title

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

            text: filterView.title
            font.pixelSize: 16
            font.bold: true
            font.underline: ( parent.state == "selected" || parent.state == "hovered" )
        }

        /* FIXME: add an "all" button
           filter.filtering is a bool indicating its state
           filter.clear() is the method that should be used when clicking on it */
    }

    GridViewWithSpacing {
        id: filters

        columns: ( filterView.filterModel.id == "genre"
                  || filterView.filterModel.id == "modified" ) ? 3 : 2
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
                text: item.name
                checked: item.active
                onClicked: item.active = !item.active
            }
        }

        model: filterView.filterModel.options
    }
}
