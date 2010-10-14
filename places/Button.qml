import Qt 4.7

Item {
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

        onPressed: {
            parent.state = "pressed"
        }

        onReleased: {
            if(containsMouse)
                parent.state = "selected";
            else if(parent.activeFocus)
                parent.state = "selected"
            else
                parent.state = "default"
        }

        onEntered: {
            parent.state = "selected"
        }

        onExited: {
            if(parent.activeFocus)
                parent.state = "selected"
            else
                parent.state = "default"
        }
    }

    onActiveFocusChanged: {
        if(activeFocus)
            state = "selected"
        else
            state = "default"
    }

    Keys.onPressed: {
        if (event.key == Qt.Key_Return) {
            clicked()
            event.accepted = true;
        }
    }

    Rectangle {
        anchors.fill: parent
        opacity: parent.state == "selected" || parent.state == "pressed" ? 1.0 : 0.0

        color: parent.state == "pressed" ? "#ffffffff" : "#00000000"
        border.color: "#cccccc"
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
}
