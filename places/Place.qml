import Qt 4.7
import dee 1.0
import UnityApplications 1.0 /* Necessary for the ImageProvider serving image://icons */
import UnityPlaces 1.0
import "utils.js" as Utils

Page {
    id: place

    property string dBusObjectPath
    property string dBusObjectPathPlaceEntry

    property string dBusService: "com.canonical.Unity." + name + "Place"
    property string dBusDeePrefix: "/com/canonical/dee/model/com/canonical/Unity/" + name + "Place/"

    /* The PlaceEntry D-Bus API does not expose properties but only setters.
       We make sure the setters are always synchronised with our local properties. */
    onActiveChanged: {
        place_entry.SetActive(active)
        /* PlaceEntry.SetActiveSection needs to be called after PlaceEntry.SetActive
           in order for it to have an effect. */
        place_entry.SetActiveSection(activeSection)
    }
    onActiveSectionChanged: place_entry.SetActiveSection(activeSection)
    onSearchQueryChanged: place_entry.SetSearch(searchQuery, [])

    /* Sections model containing the list of available sections for the place */
    sections: DeeListModel {
                   service: dBusService
                   objectPath: dBusDeePrefix ? dBusDeePrefix + "SectionsModel" : ""
              }

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

    /* Tries various methods to trigger a sensible action for the given 'uri'.
       First it asks the place backend via its 'Activate' method. If that fails
       it does its best to select a relevant action for the uri's schema. If it
       has no understanding of the given schema it falls back on asking Qt to
       open the uri.
    */
    function activate(uri) {
        if(!dBusInterface.Activate(uri)) {
            var matches = uri.match("^(.*)(?:://)(.*)$")
            var schema = matches[1]
            var path = matches[2]
            if(schema == "application") {
                Utils.launchApplicationFromDesktopFile(path, parent)
            }
            else {
                console.log("FIXME: Possibly no handler for schema \'%1\'".arg(schema))
                console.log("Trying to open", uri)
                /* Try our luck */
                /* FIXME: uri seems already escaped though
                          Qt.openUrlExternally tries to escape it */
                Qt.openUrlExternally(uri)
            }
        }
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
        /* Take all available horizontal space if the scrollbar is invisible */
        anchors.right: scrollbar.opacity > 0 ? scrollbar.left : parent.right

        clip: true
        /* FIXME: proper spacing cannot be set because of the hack in Group.qml
           whereby empty groups are still in the list but invisible and of
           height 0.
        */
        //spacing: 31

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

        /* The group's delegate is chosen dynamically depending on what
           groupRenderer is returned by the GroupsModel.

           Each groupRenderer should have a corresponding QML file with the
           same name that will be used as delegate.
           For example:

           If groupRenderer == 'UnityShowcaseRenderer' then it will look for
           the file 'UnityShowcaseRenderer.qml' and use it to render the group.
        */
        delegate: Loader {
            property string groupRenderer: column_0
            property string displayName: column_1
            property string iconHint: column_2
            property int groupId: index

            source: groupRenderer ? groupRenderer+".qml" : ""
            onStatusChanged: {
                if (status == Loader.Error)
                    console.log("Failed to load renderer", groupRenderer)
            }

            /* -2 is here to prevent clipping of the group; it looks like a bug */
            width: ListView.view.width-2

            /* Model that will be used by the group's delegate */
            QSortFilterProxyModelQML {
                id: group_model

                model: resultsModel

                /* resultsModel contains data for all the groups of a given Place.
                   Each row has a column (the second one) containing the id of
                   the group it belongs to (groupId).
                */
                filterRole: 2 /* second column (see above comment) */
                filterRegExp: RegExp("^%1$".arg(groupId)) /* exact match */

                /* Maximum number of items in the model; -1 is unlimited */
                limit: item.modelCountLimit
            }

            onLoaded: {
                item.displayName = displayName
                item.iconHint = iconHint
                item.model = group_model
                item.place = place
            }
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

        targetFlickable: place_results

        /* Hide the scrollbar if there is less than a page of results */
        opacity: targetFlickable.visibleArea.heightRatio < 1.0 ? 1.0 : 0.0
        Behavior on opacity {NumberAnimation {duration: 100}}
    }
}
