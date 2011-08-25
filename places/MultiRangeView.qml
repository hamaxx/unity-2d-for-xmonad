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
    property int cellWidth: Math.floor(width / count)

    /* Keep track of active cells with array, from which determine index
        of leftmost and rightmost active cells.*/
    property variant activeCells: Array(count)
    property int leftActiveCell: -1
    property int rightActiveCell: -1

    function cellActivationChanged(cellId, state) {
        var tmp = activeCells
        tmp[cellId] = state
        activeCells = tmp
        setSelectionBarPosition()
    }

    function setSelectionBarPosition() {
        leftActiveCell = activeCells.indexOf(true)
        rightActiveCell = activeCells.lastIndexOf(true);
    }

    Rectangle {
        id: container

        /* FIXME: Rectangle's borders grow half inside and half outside of the
           rectangle. In order to avoid it being clipped, we adjust its size
           and position depending on its border's width.

           Ref.: http://lists.qt.nokia.com/pipermail/qt-qml/2010-May/000264.html
        */
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

        anchors.fill: parent
        orientation: ListView.Horizontal
        focus: true
        boundsBehavior: Flickable.StopAtBounds

        delegate: MultiRangeButton {
            height: ListView.view.height
            width: cellWidth
            text: item.name
            checked: item.active
            onClicked: item.active = !item.active
            onCheckedChanged: cellActivationChanged(model.index, checked)
            isLast: ( model.index == count-1 )
            Component.onCompleted: cellActivationChanged(model.index, item.active)
        }
    }

    MultiRangeSelectionBar {
        id: selectionBar

        visible: ( leftActiveCell != -1 && rightActiveCell != -1 )
        isFirst: ( leftActiveCell == 0 )
        isLast: ( rightActiveCell == count-1 )
        leftPos: ( leftActiveCell == 0 ) ? 0 : leftActiveCell*cellWidth-2
        /* Hack: Rounding errors can mean the right of the selectionBar fails
           to wholly cover the container Rectangle (and its border) */
        rightPos: ( rightActiveCell == count-1 ) ? parent.width : (rightActiveCell+1)*cellWidth+3
    }
}
