import Qt 4.7

Button {
    property alias label: label.text

    TextCustom {
        id: label

        anchors.fill: parent
        color: parent.state == "pressed" ? "#444444" : "#ffffff"
        elide: Text.ElideMiddle
        horizontalAlignment: Text.AlignHCenter
    }
}
