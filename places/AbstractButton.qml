import Qt 4.7

Item {
    property bool enabled: true

    signal clicked

    MouseArea {
        id: mouse_area

        /* FIXME: workaround double click bug
                  http://bugreports.qt.nokia.com/browse/QTBUG-12250 */
        property bool double_clicked: false

        enabled: parent.enabled
        hoverEnabled: parent.enabled
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
}
