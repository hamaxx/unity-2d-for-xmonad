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
            width: 58; height: 54
            icon: "image://icons/"+application.icon
            running: application.running
            active: application.active
            urgent: application.urgent
            sticky: application.sticky
            launching: application.launching
            onClicked: {
                if (mouse.button == Qt.LeftButton) {
                    if (active)
                        application.expose()
                    else if (running && application.has_visible_window)
                        application.show()
                    else
                        application.launch()
                }
                else if (mouse.button == Qt.RightButton) {
                    menu.show(y + height / 2, application)
                }
            }
        }
    }
}
