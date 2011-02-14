import Qt 4.7

/* AutoScrollingListView

   Implements a ListView that have two mouse sensitive areas, one at the top and one at the bottom.
   While the cursors is in these areas the list automatically scrolls in the direction where the area
   is until the cursor exits the area or the scrolling reaches the edge.

   The property 'autoScrollSize' controls how tall are the two sensitive areas.
   The speed of the scrolling is controlled by 'autoScrollVelocity'.
   You can check if we are currently autoscrolling using the 'autoScrolling' property.

*/
ListView {
    id: list
    property int autoScrollSize
    property real autoScrollVelocity
    property bool isAutoScrolling: scrollUp.running || scrollDown.running

    MouseArea {
        id: scrollZoneTop
        width: parent.width
        height: autoScrollSize
        anchors.top: parent.top
        hoverEnabled: true
    }

    MouseArea {
        id: scrollZoneBottom
        width: parent.width
        height: autoScrollSize
        anchors.bottom: parent.bottom
        hoverEnabled: true
    }

    SmoothedAnimation {
        id: scrollUp
        target: list
        property: "contentY"
        to: 0
        velocity: autoScrollVelocity
        running: scrollZoneTop.containsMouse
    }

    SmoothedAnimation {
        id: scrollDown
        target: list
        property: "contentY"
        to: contentHeight - height
        velocity: autoScrollVelocity
        running: scrollZoneBottom.containsMouse
    }

    /* The code below this comment is only needed as a workaround for a strange behavior
       (or bug) in QML. Essentially MouseEvents have an accepted property, but in most cases
       it doesn't matter what you set it to: the event is always accepted and not propagated
       further. This prevents mouse activity on the two MouseAreas to flow down to the list
       items. Therefore we need to forward the events we need (entered, exited, clicked).
    */
}
