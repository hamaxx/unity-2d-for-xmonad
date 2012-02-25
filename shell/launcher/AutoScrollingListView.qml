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

/* AutoScrollingListView

   ListView that has two mouse sensitive areas, one at the top and one at the bottom.
   While the cursors is in these areas the list automatically scrolls in the direction
   where the area is until the cursor exits the area or the scrolling reaches the edge.

   The property 'autoScrollSize' controls how tall are the two sensitive areas.
   The speed of the scrolling is controlled by 'autoScrollVelocity'.
   You can check if we are currently autoscrolling using the 'autoScrolling' property.
*/
ListView {
    id: list
    property int autoScrollSize
    property real autoScrollVelocity
    property bool autoScrolling: scrollUp.running || scrollDown.running

    MouseArea {
        id: scrollZoneTop
        width: parent.width
        height: autoScrollSize
        anchors.top: parent.top
        hoverEnabled: true
        z: overlayZ
    }

    MouseArea {
        id: scrollZoneBottom
        width: parent.width
        height: autoScrollSize
        anchors.bottom: parent.bottom
        hoverEnabled: true
        z: overlayZ
    }

    SmoothedAnimation {
        id: scrollUp
        target: list
        property: "contentY"
        to: 0
        velocity: autoScrollVelocity
        running: scrollZoneTop.containsMouse || draggingOnScrollZoneTop
    }

    SmoothedAnimation {
        id: scrollDown
        target: list
        property: "contentY"
        to: contentHeight - height
        velocity: autoScrollVelocity
        running: (scrollZoneBottom.containsMouse && contentHeight > height)
                 || draggingOnScrollZoneBottom
    }

    function stopAutoScrolling() {
        scrollUp.stop()
        scrollDown.stop()
    }

    /* The code below this comment is only needed as a workaround for a strange behavior
       (or bug) in QML. Essentially MouseEvents have an accepted property, but in most cases
       it doesn't matter what you set it to: the event is always accepted and not propagated
       further. This prevents mouse activity on the two MouseAreas to flow down to the list
       items. Therefore we need to forward the events we need (entered, exited, clicked).
       The QT bug reference is:
       http://bugreports.qt.nokia.com/browse/QTBUG-13007?focusedCommentId=137123
    */
    property variant itemBelow: null

    Connections {
        target: scrollZoneTop
        onEntered: updateItemBelow(scrollZoneTop)
        onExited: { if (itemBelow) itemBelow.exited(); itemBelow = null }
        onPositionChanged: updateItemBelow(scrollZoneTop)
        onClicked: forwardClick(scrollZoneTop, mouse)
    }

    Connections {
        target: scrollZoneBottom
        onEntered: updateItemBelow(scrollZoneBottom)
        onExited: { if (itemBelow) itemBelow.exited(); itemBelow = null }
        onPositionChanged: updateItemBelow(scrollZoneBottom)
        onClicked: forwardClick(scrollZoneBottom, mouse)
    }

    onContentYChanged: updateItemBelow((scrollZoneBottom.containsMouse) ? scrollZoneBottom :
                                       (scrollZoneTop.containsMouse) ? scrollZoneTop : null)

    function updateItemBelow(zone) {
        if (zone == null) return
        var point = zone.mapToItem(list.contentItem, zone.mouseX, zone.mouseY)
        var item = list.contentItem.childAt(point.x, point.y)
        /* Ignore header, footer and any item that doesn't have the signals
           we need to forward */
        if (item && (typeof(item.entered) != "function" ||
                     typeof(item.exited) != "function")) item = null;

        if (item == null) {
            if (itemBelow != null) {
                itemBelow.exited()
                itemBelow = null
            }
        } else {
            if (itemBelow != item && itemBelow != null) itemBelow.exited()
            item.entered()
            itemBelow = item
        }
    }

    /* FIXME: We forward the click but the coordinates will be wrong since they are expressed in the
       coordinate system of the MouseArea where it happened. However they are readonly properties in
       the MouseEvent so we can't update them with the translated values.
       It should be ok for now since we don't use them for now.
    */
    function forwardClick(zone, mouse) {
        var point = zone.mapToItem(list.contentItem, zone.mouseX, zone.mouseY)
        var item = list.contentItem.childAt(point.x, point.y)
        if (item && typeof(item.clicked) == "function") item.clicked(mouse)
    }

    /* If drag and drop reordering is enabled for this list, this will not
       be null. Normally we could keep autoscrolling and drag and drop reordering
       entirely separated, but due to the QT issue explained above we can't let
       the mouse events "bubble down" from the d'n'd MouseArea to the autoscroll
       MouseAreas, so we need a workaround like this one. */
    property variant dragAndDrop: null
    property bool draggingOnScrollZoneTop: dragAndDrop != null && dragAndDrop.draggedTileId != "" &&
                                           dragAndDrop.mouseY >= scrollZoneTop.y &&
                                           dragAndDrop.mouseY <= scrollZoneTop.y + autoScrollSize
    property bool draggingOnScrollZoneBottom: dragAndDrop != null && dragAndDrop.draggedTileId != "" &&
                                              dragAndDrop.mouseY >= scrollZoneBottom.y &&
                                              dragAndDrop.mouseY <= scrollZoneBottom.y + autoScrollSize
}
