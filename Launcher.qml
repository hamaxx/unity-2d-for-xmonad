import Qt 4.7

Item {
    Image {
        id: background

        anchors.fill: parent
        fillMode: Image.Tile
        source: "artwork/honeycomb.png"
    }

    ListView {
        anchors.fill: parent

        anchors.horizontalCenter: parent.horizontalCenter
        anchors.verticalCenter: parent.verticalCenter
    //    cellWidth: 220
    //    cellHeight: 215
    //    flow: GridView.LeftToRight
    //    highlightRangeMode: GridView.StrictlyEnforceRange
    //    preferredHighlightBegin: 40
    //    preferredHighlightEnd: 40


        model: ListModel {

            ListElement {
                desktop_file: "/usr/share/applications/gcalctool.desktop"
            }
            ListElement {
                desktop_file: "/usr/share/applications/evince.desktop"
            }
            ListElement {
                desktop_file: "/usr/share/applications/chromium-browser.desktop"
            }
            ListElement {
                desktop_file: "/usr/share/applications/totem.desktop"
            }
        }

        focus: true

        /* This delegatae is in fact never used and here for demonstration
           purposes only. */
        delegate: Application {
            property variant app: QLauncherApplication {desktop_file: model.desktop_file}

            width: 60; height: 60
            icon: app.icon_path
            running: app.running
            onClicked: app.launch()
        }
    }
}
