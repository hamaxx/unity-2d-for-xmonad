import Qt 4.7
import QtDee 1.0
import UnityApplications 1.0 /* Necessary for the ImageProvider serving image://icons */
import UnityPlaces 1.0

Item {
    id: place

    property string name
    property string dBusObjectPath

    property string dBusService: "com.canonical.Unity." + name + "Place"
    property string dBusDeePrefix: "/com/canonical/dee/model/com/canonical/Unity/" + name + "Place/"

    /* FIXME: this is a bit of a hack due to the lack of D-Bus property
              giving the current section id for a place
    */
    property int activeSection

    function setActiveSection(section) {
        activeSection = section
        place_entry.SetActiveSection(section)
    }

    function search(query) {
        place_entry.SetSearch(query, [])
    }

    UnityPlaceEntry {
        id: place_entry

        service: dBusService
        objectPath: dBusObjectPath
    }

    GridView {
        id: results

        property int delegate_width: 165
        property int delegate_height: 80
        property int horizontal_spacing: 20
        property int vertical_spacing: 25

        anchors.top: parent.top
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        anchors.right: scrollbar.left

        cellWidth: delegate_width+horizontal_spacing
        cellHeight: delegate_height+vertical_spacing
        clip: true

        delegate: Result {
            width: GridView.view.delegate_width
            height: GridView.view.delegate_height
            label: column_4
            icon: "image://icons/"+column_1
        }

        model: DeeListModel {
            service: dBusService
            objectPath: dBusDeePrefix +"ResultsModel"
        }
    }

    Scrollbar {
        id: scrollbar

        anchors.top: parent.top
        anchors.bottom: parent.bottom
        anchors.right: parent.right
        width: 10

        position: results.visibleArea.yPosition
        pageSize: results.visibleArea.heightRatio
    }
}
