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

/* When added as a child of a ListView and the listview itself is set to the
   'list' property it will make it possible to use drag’n’drop to re-order
   the items in the list. */

// FIXME: flicking the list fast exhibits unpleasant visual artifacts:
// the y coordinate of the looseItems is correct, however they are not
// re-drawn at the correct position until the mouse cursor is moved
// further. This may be a bug in QML.
// Ref: https://bugs.launchpad.net/unity-2d/+bug/727082.
MouseArea {
    id: dnd
    anchors.fill: parent

    property variant list

    /* list index of the tile being dragged */
    property int draggedTileIndex
    /* id (desktop file path) of the tile being dragged */
    property string draggedTileId: ""
    /* absolute mouse coordinates in the list */
    /* list.contentY is artificially introduced in the calculation to make sure
       the coordinates are updated when the list is being flicked.
       Ref: https://bugs.launchpad.net/unity-2d/+bug/731449 */
    property variant listCoordinates: mapToItem(list.contentItem, mouseX, mouseY + 0 * list.contentY)
    /* list index of the tile underneath the cursor */
    property int tileAtCursorIndex: list.indexAt(listCoordinates.x, listCoordinates.y)
    property bool ignoreNextClick: false

    Timer {
        id: longPressDelay
        /* The standard threshold for long presses is hard-coded to 800ms
           (http://doc.qt.nokia.com/qml-mousearea.html#onPressAndHold-signal).
           This value is too high for our use case. */
        interval: 500 /* in milliseconds */
        onTriggered: {
            if (list.moving) return
            dnd.parent.interactive = false
            var id = items.get(dnd.draggedTileIndex).desktop_file
            if (id != undefined) dnd.draggedTileId = id
        }
    }
    onPressed: {
        /* tileAtCursorIndex is not valid yet because the mouse area is
           not sensitive to hovering (if it were, it would eat hover
           events for other mouse areas below, which is not desired). */
        var coord = mapToItem(list.contentItem, mouse.x, mouse.y)
        draggedTileIndex = list.indexAt(coord.x, coord.y)
        longPressDelay.start()
    }
    function drop() {
        longPressDelay.stop()
        draggedTileId = ""
        parent.interactive = true
    }
    onCanceled: {
        /* The parent flickable (dnd.parent) can steal the mouse grab and when
           that happens onReleased will not be triggerred and the dragging will
           not be stopped anymore. Do it manually.
           Ref.: https://bugs.launchpad.net/unity-2d/+bug/768812
        */
        drop()
    }
    onReleased: {
        if (draggedTileId != "") {
            drop()
            ignoreNextClick = true
        }
    }
    onClicked: {
        /* a "clicked" signal is emitted whenever the mouse is pressed and
           released in a MouseArea, regardless of any intervening mouse
           movements; for this reason, one is emitted also after a DragAndDrop
           action, and we clearly don't want to process it.
           The ignoreNextClick flag serves this purpose, and is set in the
           onRelease handler. */
        if (ignoreNextClick) {
            ignoreNextClick = false
            return
        }
        /* Forward the click to the launcher item below. */
        var point = mapToItem(list.contentItem, mouse.x, mouse.y)
        var item = list.contentItem.childAt(point.x, point.y)
        /* FIXME: the coordinates of the mouse event forwarded are
           incorrect. Luckily, it’s acceptable as they are not used in
           the handler anyway. */
        if (item && typeof(item.clicked) == "function") item.clicked(mouse)
    }
    /* This handler is necessary to avoid receiving duplicate "clicked"
       signals. */
    onDoubleClicked: {
    }
    onExited: drop()
    function reorder() {
        if (draggedTileId != "" && tileAtCursorIndex != -1 && tileAtCursorIndex != draggedTileIndex) {
            items.move(draggedTileIndex, tileAtCursorIndex, 1)
            draggedTileIndex = tileAtCursorIndex
        }
    }
    onPositionChanged: reorder()
    Connections {
        /* Also trigger a re-ordering when the mouse is not moving but the list
           is auto-scrolling. */
        target: list
        onContentYChanged: reorder()
    }
}
