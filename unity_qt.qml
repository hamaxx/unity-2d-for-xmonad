import Qt 4.7

Item {
    width: 1024; height: 768

    Image {
        id: background

        anchors.fill: parent
        source: "artwork/background.jpg"
    }

    Launcher {
        id: launcher

        anchors.left: parent.left
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        width: 60
    }
}
