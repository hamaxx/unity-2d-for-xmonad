import Qt 4.7

/* AutoScrollingListView

   ListView that has two mouse sensitive areas, one at the top and one at the bottom.
   While the cursors is in these areas the list automatically scrolls in the direction
   where the area is until the cursor exits the area or the scrolling reaches the edge.

   In addition to this, it is possible to add padding at the top and/or bottom of the
   list that takes part in the scrolling (contrary to what would happen by adding this
   extra space using margins).

   The property 'autoScrollSize' controls how tall are the two sensitive areas.
   The speed of the scrolling is controlled by 'autoScrollVelocity'.
   You can check if we are currently autoscrolling using the 'autoScrolling' property.
   The properties 'paddingTop' and 'paddingBottom' control the size of the padding.
*/
ListView {
    id: list
    property int autoScrollSize
    property real autoScrollVelocity
    property bool autoScrolling: scrollUp.running || scrollDown.running
    property int paddingTop: 0
    property int paddingBottom: 0

    /* We implement the padding by adding an empty footer and header
       of the requested size. The header and footer are inside the ListView
       (contrary to the use of anchors.margins, which adds space outside).
       However they are not included in contentHeight, therefore we need to
       adjust the rest of the autoscrolling code to take them into account.
    */
    header: Item { height: list.paddingTop }
    footer: Item { height: list.paddingBottom }

    /* This is necessary to initially scroll to a position where the header
       is visible. Otherwise the edge of the first item would be at the top
       of the list */
    contentY: 0 - paddingTop

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
        to: 0 - paddingTop
        velocity: autoScrollVelocity
        running: scrollZoneTop.containsMouse
                 /* When a drag is in progress to re-order applications, scrollZoneTop doesn’t receive mouse events. */
                 || (dnd.currentId != "" && dnd.mouseY >= scrollZoneTop.y && dnd.mouseY <= scrollZoneTop.y + autoScrollSize)
    }

    SmoothedAnimation {
        id: scrollDown
        target: list
        property: "contentY"
        to: contentHeight + paddingBottom - height
        velocity: autoScrollVelocity
        running: (scrollZoneBottom.containsMouse && contentHeight + paddingBottom > height)
                 /* When a drag is in progress to re-order applications, scrollZoneBottom doesn’t receive mouse events. */
                 || (dnd.currentId != "" && dnd.mouseY >= scrollZoneBottom.y && dnd.mouseY <= scrollZoneBottom.y + autoScrollSize)
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
}
