import Qt 4.7
import launcher 1.0

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

        model: launcher_applications

        delegate: Application {
            width: 60; height: 60
            icon: "image://icons/"+application.icon
            running: application.running
            onClicked: if(!running) application.launch()
        }
    }
}
