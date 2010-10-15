import Qt 4.7

Button {
    property alias label: label.text
    property int horizontalPadding: 0
    property int verticalPadding: 0

    width: label.width + horizontalPadding*2
    height: label.height + verticalPadding*2

    TextCustom {
        id: label

        width: paintedWidth
        height: paintedHeight
        anchors.verticalCenter: parent.verticalCenter
        anchors.horizontalCenter: parent.horizontalCenter

        color: parent.state == "pressed" ? "#444444" : "#ffffff"
        horizontalAlignment: Text.AlignHCenter
    }
}
