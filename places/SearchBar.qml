import Qt 4.7
import QtDee 1.0

Item {
    BorderImage {
        id: background

        anchors.fill: parent
        source: "artwork/border_glow.png"
        smooth: false
        border.left: 11
        border.right: 11
        border.top: 12
        border.bottom: 12
        /* It might be more efficient to have a png with a bigger transparent
           middle and setting the tile modes to Repeat */
        horizontalTileMode: BorderImage.Stretch
        verticalTileMode: BorderImage.Stretch

        Image {
            anchors.fill: parent
            anchors.margins: 6
            source: "artwork/checker.png"
            fillMode: Image.Tile
        }
    }

    ListView {
        id: sections

        interactive: false
        orientation: ListView.Horizontal

        anchors.left: parent.left
        anchors.leftMargin: 375
        anchors.right: parent.right
        anchors.rightMargin: 15
        spacing: 10

        height: parent.height

        delegate: Section {
            anchors.verticalCenter: parent.verticalCenter
            horizontalPadding: 4
            verticalPadding: 3
            label: column_0

            onClicked: {
                ListView.view.currentIndex = model.index
                current_page.setActiveSection(model.index)
            }
        }

        model: DeeListModel {
            service: current_page.dBusService
            objectPath: current_page.dBusDeePrefix + "SectionsModel"
        }
    }
}
