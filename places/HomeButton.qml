import Qt 4.7

Button {
    property alias icon: icon.source
    property alias label: label.text
    property alias iconSourceSize: icon.sourceSize

    width: 160
    height: 172

    Item {
        anchors.right: parent.right
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.bottom: label.bottom
        anchors.bottomMargin: 30

        Image {
            id: icon

            width: sourceSize.width
            height: sourceSize.height

            anchors.centerIn: parent
            fillMode: Image.PreserveAspectFit

            asynchronous: true
            opacity: status == Image.Ready ? 1 : 0
            Behavior on opacity {NumberAnimation {duration: 200; easing.type: Easing.InOutQuad}}
        }
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
