import Qt 4.7
import QtDee 1.0
import UnityApplications 1.0 /* Necessary for the ImageProvider serving image://icons */
import UnityPlaces 1.0

Page {
    id: place

    property string name
    property string dBusObjectPath

    property string dBusService: "com.canonical.Unity." + name + "Place"
    property string dBusDeePrefix: "/com/canonical/dee/model/com/canonical/Unity/" + name + "Place/"

    /* FIXME: this is a bit of a hack due to the lack of D-Bus property
              giving the current section id for a place
    */
    property int activeSection
    property bool hasSections: true

    /* ResultsModel containing data for all the Groups. Each Group will filter
       it locally. */
    property variant resultsModel: DeeListModel {
                                        service: dBusService
                                        objectPath: dBusDeePrefix +"ResultsModel"
                                   }


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

    ListView {
        id: results

        anchors.top: parent.top
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        anchors.right: scrollbar.left
        clip: true

        orientation: ListView.Vertical

        delegate: Group {
            id: group

            /* -2 is here to prevent clipping of the group; it looks like a bug */
            width: ListView.view.width-2
            groupNumber: index
            label: column_1
            icon: column_2
            placeResultsModel: resultsModel
        }

        model: DeeListModel {
            service: dBusService
            objectPath: dBusDeePrefix + "GroupsModel"
        }
    }

    Scrollbar {
        id: scrollbar

        anchors.top: parent.top
        anchors.bottom: parent.bottom
        anchors.right: parent.right
        width: 10

        /* FIXME: Because of the nesting of GridViews inside the ListView, these
                  values do not provide the expected result.
                  Deactivating the scrollbar for now.
        */
        visible: false
        position: results.visibleArea.yPosition
        pageSize: results.visibleArea.heightRatio
    }
}
