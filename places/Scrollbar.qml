import Qt 4.7

Item {
    property real position
    property real pageSize

    BorderImage {
        id: background

        anchors.fill: parent
        source: "artwork/scrollbar_background.png"
        smooth: false
        border.top: 6
        border.bottom: 6
        border.left: 0
        border.right: 0
        horizontalTileMode: BorderImage.Stretch
        verticalTileMode: BorderImage.Repeat
    }

    /* FIXME: the handle does not look like Unity's; it is missing:
               - the glow
               - the 3 small marks in the middle
    */
    Item {
        id: handle

        anchors.left: parent.left
        anchors.right: parent.right

        /* FIXME: ugly code */
        y: {
            if(parent.position <= 0)
                return 0
            else if(parent.position + parent.pageSize >= 1)
                return (1-parent.pageSize) * (parent.height-2)
            else
                return parent.position * (parent.height-2)
        }

        height: parent.pageSize * (parent.height-2)

        Rectangle {
            anchors.fill: parent
            anchors.rightMargin: 1

            color: mouse.pressed ? "#ffffffff" : "#00000000"
            border.color: "#cccccc"
            border.width: 1
            radius: 5

            Image {
                fillMode: Image.Tile
                anchors.fill: parent
                source: "artwork/button_background.png"
                smooth: false

                opacity: mouse.containsMouse || mouse.pressed ? 1.0 : 0.0
                Behavior on opacity {NumberAnimation {duration: 100}}
            }
        }

        MouseArea {
            id: mouse

            hoverEnabled: true
            anchors.fill: parent
            /* FIXME: missing dragging of the handle */
        }
    }
}
