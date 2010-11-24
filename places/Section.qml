import Qt 4.7

AbstractButton {
    property alias label: label.text
    property int horizontalPadding: 0
    property int verticalPadding: 0
    property bool isActiveSection: false
    property bool activeOrPressed: isActiveSection || state == "pressed"

    width: label.width + horizontalPadding*2
    height: label.height + verticalPadding*2

    Rectangle {
        anchors.fill: parent
        opacity: activeOrPressed || parent.state == "selected" ? 1.0 : 0.0

        color: activeOrPressed ? "#ffffffff" : "#00000000"
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

        color: activeOrPressed ? "#444444" : "#ffffff"
        horizontalAlignment: Text.AlignHCenter
    }
}
