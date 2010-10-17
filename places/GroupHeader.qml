import Qt 4.7

Button {
    property alias icon: icon.source
    property alias label: label.text
    property bool folded: true

    Image {
        id: icon

        width: 22
        height: 22
        anchors.verticalCenter: parent.verticalCenter
        anchors.left: parent.left
        anchors.leftMargin: 5
        fillMode: Image.PreserveAspectFit
        sourceSize.width: width
        sourceSize.height: height
    }

    TextCustom {
        id: label

        color: parent.state == "pressed" ? "#444444" : "#ffffff"
        font.pixelSize: 16
        anchors.top: parent.top
        anchors.topMargin: 5
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 5
        anchors.left: icon.right
        anchors.leftMargin: 5
        width: paintedWidth
    }

    Image {
        id: arrow

        rotation: folded ? 0 : 90
        source: "artwork/arrow.png"
        width: 7
        height: 7
        anchors.verticalCenter: label.verticalCenter
        anchors.left: label.right
        anchors.leftMargin: 6
        fillMode: Image.PreserveAspectFit
        sourceSize.width: width
        sourceSize.height: height
    }
}
