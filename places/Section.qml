import Qt 4.7

AbstractButton {
    property alias label: label.text
    property bool active: false
    property int horizontalPadding: 0
    property int verticalPadding: 0

    width: label.width + horizontalPadding*2
    height: label.height + verticalPadding*2

    Rectangle {
        anchors.fill: parent
        opacity: active || parent.state == "selected" || parent.state == "pressed" ? 1.0 : 0.0

        color: active || parent.state == "pressed" ? "#ffffffff" : "#00000000"
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

    TextCustom {
        id: label

        width: paintedWidth
        height: paintedHeight
        anchors.verticalCenter: parent.verticalCenter
        anchors.horizontalCenter: parent.horizontalCenter

        color: active || parent.state == "pressed" ? "#444444" : "#ffffff"
        horizontalAlignment: Text.AlignHCenter
    }
}
