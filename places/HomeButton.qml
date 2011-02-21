import Qt 4.7

Button {
    property alias icon: icon.source
    property alias label: label.text

    width: 160
    height: 172

    Image {
        id: icon

        anchors.top: parent.top
        anchors.horizontalCenter: parent.horizontalCenter
        width: 128
        height: 128
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
        anchors.rightMargin: 5
        anchors.leftMargin: 7
        font.underline: parent.activeFocus
        font.bold: true
        font.pixelSize: 16
    }
}
