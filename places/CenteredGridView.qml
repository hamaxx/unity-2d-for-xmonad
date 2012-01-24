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

GridView {
    property int minHorizontalSpacing: 0
    property int minVerticalSpacing: 0
    property int delegateWidth: 100
    property int delegateHeight: 100

    /* Manually handle key presses so that when the 'interactive' property is
       set to false the keyboard navigation still works.

       Ref.: https://bugreports.qt.nokia.com/browse/QTBUG-17051
    */
    function selectChild(index) {
        if (index < 0 || index >= count) return false
        currentIndex = index
        return true
    }

    /* FIXME: HACK to workaround GridView's layout issues in Qt 4.7.4.
       Ref.: https://bugreports.qt.nokia.com/browse/QTBUG-21452
    */
    onWidthChanged: {
        var y = contentY
        contentY = 0
        contentY = y
    }

    Keys.onPressed: if (handleKeyPress(event.key)) event.accepted = true
    function handleKeyPress(key) {
        switch (key) {
        case Qt.Key_Right:
            return selectChild(currentIndex+1)
        case Qt.Key_Left:
            return selectChild(currentIndex-1)
        case Qt.Key_Up:
            return selectChild(currentIndex-cellsPerRow)
        case Qt.Key_Down:
            return selectChild(currentIndex+cellsPerRow)
        }
        return false
    }

    /* Compute the number of cells per row and column so that:
        - the items are always centered horizontally within the grid (= same
          margins on the right and left sides of the grid) when flow is set
          to LeftToRight
        - the items are always centered vertically within the grid (= same
          margins on the top and bottom sides of the grid) when flow is set
          to TopToBottom
        - the spacing between items is kept as small as possible but always
          greater than minHorizontalSpacing and minVerticalSpacing
    */
    property int cellsPerRow: flow == GridView.LeftToRight ? Math.floor(width/(minHorizontalSpacing+delegateWidth)) : 0
    property int cellsPerColumn: flow == GridView.LeftToRight ? 0 : Math.floor(height/(minVerticalSpacing+delegateHeight))
    property int horizontalSpacing: flow == GridView.LeftToRight ? Math.floor(width/cellsPerRow-delegateWidth) : minHorizontalSpacing
    property int verticalSpacing: flow == GridView.LeftToRight ? minVerticalSpacing : Math.floor(height/cellsPerColumn-delegateHeight)

    cellWidth: delegateWidth+horizontalSpacing
    cellHeight: delegateHeight+verticalSpacing
}
