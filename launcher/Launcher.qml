import Qt 4.7
import UnityApplications 1.0

Item {
    width: 58
    height: 1024

    Image {
        id: background

        anchors.fill: parent
        fillMode: Image.TileVertically
        source: "/usr/share/unity/themes/launcher_background_middle.png"
    }

    ListView {
        anchors.fill: parent
        focus: true

        /* applications is exposed by UnityApplications */
        model: applications

        delegate: Application {
            id: wrapper

            width: 58; height: 54
            icon: "image://icons/"+application.icon
            running: application.running
            active: application.active
            urgent: application.urgent
            sticky: application.sticky
            launching: application.launching
            onClicked: {
                if (mouse.button == Qt.LeftButton) {
                    menu.hide()
                    if (active)
                        application.expose()
                    else if (running && application.has_visible_window)
                        application.show()
                    else
                        application.launch()
                }
                else if (mouse.button == Qt.RightButton) {
                    menu.show_menu()
                }
            }

            onEntered: menu.show(y + height / 2, application)
            onExited: menu.hide()

            ListView.onAdd: SequentialAnimation {
                PropertyAction { target: wrapper; property: "scale"; value: 0 }
                NumberAnimation { target: wrapper; property: "height"; from: 0; to: 54; duration: 250; easing.type: Easing.InOutQuad }
                NumberAnimation { target: wrapper; property: "scale"; to: 1; duration: 250; easing.type: Easing.InOutQuad }
            }

            ListView.onRemove: SequentialAnimation {
                PropertyAction { target: wrapper; property: "ListView.delayRemove"; value: true }
                NumberAnimation { target: wrapper; property: "scale"; to: 0; duration: 250; easing.type: Easing.InOutQuad }
                NumberAnimation { target: wrapper; property: "height"; to: 0; duration: 250; easing.type: Easing.InOutQuad }
                PropertyAction { target: wrapper; property: "ListView.delayRemove"; value: false }
            }
        }
    }
}
