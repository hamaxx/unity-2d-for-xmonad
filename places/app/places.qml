import Qt 4.7
import QtDee 1.0
import UnityApplications 1.0 /* Necessary for the ImageProvider serving image://icons */
import UnityPlaces 1.0

Rectangle {
    id: place

    /* FIXME: these 2 properties need to be fed from the place configuration file
              located in /usr/share/unity/places/NAME.place
    */
    property string name: "Applications"
    property string dBusObjectPath: "/com/canonical/unity/applicationsplace/applications"

    property string dBusService: "com.canonical.Unity." + name + "Place"
    property string dBusDeePrefix: "/com/canonical/dee/model/com/canonical/Unity/" + name + "Place/"

    width: 800
    height: 600
    color: "grey"

    UnityPlaceEntry {
        id: place_entry

        service: dBusService
        objectPath: dBusObjectPath
    }

    ListView {
        id: sections

        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        height: 20
        orientation: ListView.Horizontal

        delegate: Section {
            width: 100
            height: ListView.view.height
            label: column_0

            onClicked: {
                ListView.view.currentIndex = model.index
                place_entry.SetActiveSection(model.index)
            }
        }

        model: DeeListModel {
            service: dBusService
            objectPath: dBusDeePrefix + "SectionsModel"
        }
    }

    GridView {
        id: results

        property int delegate_width: 165
        property int delegate_height: 80
        property int horizontal_spacing: 20
        property int vertical_spacing: 25

        anchors.top: sections.bottom
        anchors.topMargin: 20
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom
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
}
