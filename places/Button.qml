import Qt 4.7

AbstractButton {
    Rectangle {
        anchors.fill: parent
        anchors.bottomMargin: 1
        anchors.rightMargin: 1
        opacity: parent.state == "selected" || parent.state == "pressed" ? 1.0 : 0.0

        color: parent.state == "pressed" ? "#ffffffff" : "#00000000"
        border.color: "#cccccc"
        border.width: 1
        radius: 3

        Behavior on opacity {NumberAnimation {duration: 100}}

        Image {
            fillMode: Image.Tile
            anchors.fill: parent
            source: "artwork/button_background.png"
            smooth: false
        }
    }
}
