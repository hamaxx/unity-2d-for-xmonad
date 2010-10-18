import Qt 4.7
import QtDee 1.0
import UnityApplications 1.0 /* Necessary for the ImageProvider serving image://icons */
import UnityPlaces 1.0

Page {
    id: place

    property string name
    property string dBusObjectPath
    property string dBusObjectPathPlaceEntry

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
    property variant dBusInterface: UnityPlace {
                                         service: dBusService
                                         objectPath: dBusObjectPath
                                    }

    function setActiveSection(section) {
        /* FIXME: SetActive(false) should happen when exiting the place */
        place_entry.SetActive(false)
        place_entry.SetActive(true)
        activeSection = section
        place_entry.SetActiveSection(section)
    }

    function search(query) {
        place_entry.SetSearch(query, [])
    }

    UnityPlaceEntry {
        id: place_entry

        service: dBusService
        objectPath: dBusObjectPathPlaceEntry
    }

    ListView {
        id: place_results

        anchors.top: parent.top
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        anchors.right: scrollbar.left
        clip: true

        orientation: ListView.Vertical

        /* WARNING - HACK - FIXME
           Issue:
           User wise annoying jumps in the list are observable if cacheBuffer is
           set to 0 (which is the default value). States such as 'folded' are
           lost when scrolling a lot.

           Explanation:
           The height of the Group delegate depends on its content. However its
           content is not known until the delegate is instantiated because it
           depends on the number of results displayed by its GridView.

           Resolution:
           We set the cacheBuffer to the biggest possible int in order to make
           sure all delegates are always instantiated.
        */
        cacheBuffer: 2147483647

        delegate: Group {
            id: group

            /* -2 is here to prevent clipping of the group; it looks like a bug */
            width: ListView.view.width-2
            groupNumber: index
            label: column_1
            icon: column_2
            placeResultsModel: resultsModel
            placeDBusInterface: dBusInterface
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
        position: place_results.visibleArea.yPosition
        pageSize: place_results.visibleArea.heightRatio
    }
}
