import Qt 4.7

AbstractButton {
    id: tickBox

    property string text
    property bool ticked: false
    property bool canUntick: true

    width: childrenRect.width
    height: childrenRect.height

    TextCustom {
        id: label

        anchors.left: box.right
        anchors.leftMargin: 8
        width: paintedWidth
        anchors.top: parent.top
        font.pixelSize: 16
        text: tickBox.text
    }

    Image {
        id: box

        opacity: !canUntick && ticked ? 0 : 1
        anchors.top: parent.top
        anchors.left: parent.left
        source: "artwork/tick_box.png"
        width: sourceSize.width
        height: sourceSize.height
    }

    Image {
        id: tick

        anchors.top: box.top
        anchors.topMargin: 2
        anchors.left: box.left
        anchors.leftMargin: 3
        opacity: ticked ? 1.0 : parent.state == "selected" ? 0.4 : 0.0
        source: "artwork/tick.png"
        width: sourceSize.width
        height: sourceSize.height
    }
}
