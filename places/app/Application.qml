import Qt 4.7

Item {
    property alias icon: icon.source
    property alias label: label.text

    signal clicked

    MouseArea {
        id: mouse_area

        /* FIXME: workaround double click bug
                  http://bugreports.qt.nokia.com/browse/QTBUG-12250 */
        property bool double_clicked: false

        hoverEnabled: true
        anchors.fill: parent
        onClicked: {
            if(double_clicked)
                double_clicked = false
            else
                parent.clicked()
        }
        onDoubleClicked: {
            double_clicked = true
        }
    }

    Keys.onPressed: {
        if (event.key == Qt.Key_Return) {
            clicked()
            event.accepted = true;
        }
    }

    Rectangle {
        id: background

        anchors.fill: parent
        opacity: mouse_area.containsMouse ? 1.0 : 0.0

        color: mouse_area.pressed ? "#ffffffff" : "#00000000"
        border.color: "#dddddd"
        border.width: 1
        radius: 5

        Behavior on opacity {NumberAnimation {duration: 100}}

        Image {
            opacity: 0.3
            fillMode: Image.Tile
            anchors.fill: parent
            anchors.margins: 1
            // FIXME: wrong background
            source: "/usr/share/unity/dash_background.png"
            smooth: false
        }

    }

    Image {
        id: icon

        width: 48
        height: 48
        anchors.horizontalCenter: background.horizontalCenter
        anchors.top: parent.top
        anchors.topMargin: 4
        fillMode: Image.PreserveAspectFit
        sourceSize.width: width
        sourceSize.height: height

        asynchronous: true
        opacity: status == Image.Ready ? 1 : 0
        Behavior on opacity {NumberAnimation {duration: 200; easing.type: Easing.InOutQuad}}
    }

    TextCustom {
        id: label

        color: mouse_area.pressed ? "#444444" : "#ffffff"
        elide: Text.ElideMiddle
        horizontalAlignment: Text.AlignHCenter
        anchors.bottom: background.bottom
        anchors.right: parent.right
        anchors.left: parent.left
        anchors.bottomMargin: 5
        anchors.rightMargin: 6
        anchors.leftMargin: 6
        font.underline: parent.activeFocus
    }
}
