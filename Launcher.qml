import Qt 4.7
import UnityApplications 1.0

Item {
    width: 60
    height: 1024

    Image {
        id: background

        anchors.fill: parent
        fillMode: Image.Tile
        source: "artwork/honeycomb.png"
    }

    ListView {
        anchors.fill: parent
        focus: true

        /* applications is exposed by UnityApplications */
        model: applications

        delegate: Application {
            width: 60; height: 60
            icon: "image://icons/"+application.icon
            running: application.running
            active: application.active
            urgent: application.urgent
            launching: application.launching
            onClicked: {
                if (active)
                    application.expose()
                else if (running)
                    application.show()
                else
                    application.launch()
            }
        }
    }
}
