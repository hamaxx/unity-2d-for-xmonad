import Qt 4.7

Button {
    property alias icon: icon.source
    property alias label: label.text

    width: 128
    height: 148

    Image {
        id: icon

        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        fillMode: Image.PreserveAspectFit
        sourceSize.width: width
        sourceSize.height: height

        asynchronous: true
        opacity: status == Image.Ready ? 1 : 0
        Behavior on opacity {NumberAnimation {duration: 200; easing.type: Easing.InOutQuad}}
    }

    TextCustom {
        id: label

        color: parent.state == "pressed" ? "#444444" : "#ffffff"
        elide: Text.ElideMiddle
        horizontalAlignment: Text.AlignHCenter
        anchors.bottom: parent.bottom
        anchors.right: parent.right
        anchors.left: parent.left
        anchors.bottomMargin: 3
        anchors.rightMargin: 6
        anchors.leftMargin: 6
        font.underline: parent.activeFocus
    }
}
