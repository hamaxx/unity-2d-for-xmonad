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

FocusScope {
    id: multiRangeView

    property alias model: list.model
    property alias count: list.count
    property int cellWidth: Math.floor(container.width / count)

    property variant activeCells: Array(count)

    function cellActivationChanged(cellId, state) {
        var tmp = activeCells
        tmp[cellId] = state
        activeCells = tmp
        setSelectionBarPosition()
    }

    function setSelectionBarPosition() {
        var left = activeCells.indexOf(true)
        var right = activeCells.lastIndexOf(true)

        if (left == -1 || right == -1){
            selectionBar.visible = false
        }
        else{
            selectionBar.isFirst = (left==0)
            selectionBar.isLast = (right==count-1)
            selectionBar.leftPos = (left==0) ? 0 : left*cellWidth-2
            /* Hack: Rounding errors can mean the right of the selectionBar fails
               to wholly cover the container Rectangle (and its border) */
            selectionBar.rightPos = (right==count-1) ? parent.width : (right+1)*cellWidth+3
            selectionBar.visible = true
        }
    }

    focus: true


    Rectangle {
        id: container

        x: Math.floor(border.width / 2)
        y: Math.floor(border.width / 2)
        width: parent.width - border.width
        height: parent.height - border.width

        border.color: "#21ffffff" // 80% opaque white
        border.width: 1
        color: "transparent"
        radius: 5
    }

    ListView {
        id: list

        anchors.fill: container
        orientation: ListView.Horizontal
        focus: true
        boundsBehavior: Flickable.StopAtBounds

        delegate: MultiRangeButton {
            height: 30
            width: cellWidth
            text: item.name
            checked: item.active
            onClicked: item.active = !item.active
            onCheckedChanged: cellActivationChanged(model.index, checked)
            isLast: ( model.index == count-1 )
            Component.onCompleted: cellActivationChanged(model.index, checked)
        }
    }

    MultiRangeSelectionBar {
        id: selectionBar
    }
}
