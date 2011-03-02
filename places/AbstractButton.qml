import Qt 4.7

FocusScope {
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
    }

    state: {
        if(mouse_area.pressed)
            return "pressed"
        else if(mouse_area.containsMouse)
            return "selected"
        else if(activeFocus)
            return "selected"
        else
            return "default"
    }

    Keys.onPressed: {
        if (event.key == Qt.Key_Return) {
            clicked()
            event.accepted = true;
        }
    }
}
