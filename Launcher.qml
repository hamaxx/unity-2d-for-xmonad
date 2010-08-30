import Qt 4.7

Item {
    Image {
        id: background

        anchors.fill: parent
        fillMode: Image.Tile
        source: "artwork/honeycomb.png"
    }

    GConfItem {
        id: gconf_favorites
        key: "/desktop/unity/launcher/favorites/favorites_list"
    }

    ListView {
        anchors.fill: parent
        focus: true

        model: gconf_favorites.value

        delegate: Application {
            property variant app: QLauncherApplication {
                desktop_file: "/usr/share/applications/"+model.modelData+".desktop"
            }

            width: 60; height: 60
            icon: "image://icons/"+app.icon
            running: app.running
            onClicked: app.launch()
        }
    }
}
