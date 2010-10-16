import Qt 4.7
import QtDee 1.0

Item {
    BorderGlow {
        id: background

        anchors.fill: parent

        Image {
            anchors.fill: parent
            anchors.margins: 6
            source: "artwork/checker.png"
            fillMode: Image.Tile
        }
    }

    SearchEntry {
        id: search_entry

        width: 281
        anchors.left: parent.left
        anchors.leftMargin: 82
        anchors.top: parent.top
        anchors.topMargin: 7
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 6
    }

    ListView {
        id: sections

        visible: current_page.hasSections
        interactive: false
        orientation: ListView.Horizontal

        anchors.left: search_entry.right
        anchors.leftMargin: 14
        anchors.right: parent.right
        anchors.rightMargin: 15
        spacing: 9

        height: parent.height
        currentIndex: current_page.hasSections ? current_page.activeSection : 0

        delegate: Section {
            anchors.verticalCenter: parent.verticalCenter
            horizontalPadding: 4
            verticalPadding: 3
            label: column_0
            active: ListView.isCurrentItem

            onClicked: current_page.setActiveSection(model.index)
        }

        model: DeeListModel {
            service: current_page.hasSections ? current_page.dBusService : ""
            objectPath: current_page.hasSections ? current_page.dBusDeePrefix + "SectionsModel" : ""
        }
    }
}
