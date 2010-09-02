import Qt 4.7
import UnityApplications 1.0

Item {
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
            onClicked: if(!running) application.launch()
        }
    }
}
