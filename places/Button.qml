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
        radius: 3

        Behavior on opacity {NumberAnimation {duration: 100}}

        Image {
            fillMode: Image.Tile
            anchors.fill: parent
            source: "artwork/button_background.png"
            smooth: false
        }
    }

    /* UNUSED CODE: use of a BorderImage instead of a Rectangle
    BorderImage {
        anchors.fill: parent
        opacity: parent.state == "selected" || parent.state == "pressed" ? 1.0 : 0.0
        source: "artwork/button_border.png"
        smooth: false
        border.left: 4
        border.right: 4
        border.top: 4
        border.bottom: 4
        horizontalTileMode: BorderImage.Stretch
        verticalTileMode: BorderImage.Stretch

        Behavior on opacity {NumberAnimation {duration: 100}}

        Image {
            anchors.fill: parent
            anchors.margins: 1
            source: "artwork/button_background.png"
            fillMode: Image.Tile
        }
    }*/
}
