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
    Component.onCompleted: {
        place_entry.SetActive(active)
        place_entry.SetActiveSection(activeSection)
        place_entry.SetSearch(searchQuery, [])
        place_entry.SetGlobalSearch(globalSearchQuery, [])
    }
    onActiveChanged: {
        place_entry.SetActive(active)
        /* PlaceEntry.SetActiveSection needs to be called after PlaceEntry.SetActive
           in order for it to have an effect. */
        if (active) {
            place_entry.SetActiveSection(activeSection)
        }
    }
    onActiveSectionChanged: place_entry.SetActiveSection(activeSection)
    onSearchQueryChanged: place_entry.SetSearch(searchQuery, [])
    onGlobalSearchQueryChanged: place_entry.SetGlobalSearch(globalSearchQuery, [])

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
    property variant globalResultsModel: DeeListModel {
                                        service: dBusService
                                        objectPath: dBusDeePrefix ? dBusDeePrefix + "GlobalResultsModel" : ""
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
        var matches = uri.match("^(.*)(?:://)(.*)$")
        var scheme = matches[1]
        if (scheme == "file") {
            /* Override the files place’s default URI handler: we want the file
               manager to handle opening folders, not the dash.

               Ref: https://bugs.launchpad.net/upicek/+bug/689667
             */
             Qt.openUrlExternally(decodeURIComponent(uri))
        }
        else if (!dBusInterface.Activate(uri)) {
            if (scheme == "application") {
                var path = matches[2]
                Utils.launchApplicationFromDesktopFile(path, parent)
            }
            else {
                console.log("FIXME: Possibly no handler for scheme \'%1\'".arg(scheme))
                console.log("Trying to open", uri)
                /* Try our luck */
                Qt.openUrlExternally(decodeURIComponent(uri))
            }
        }
    }

    UnityPlaceEntry {
        id: place_entry

        service: dBusService
        objectPath: dBusObjectPathPlaceEntry
    }

    ListViewWithScrollbar {
        id: place_results

        anchors.fill: parent

        /* The group's delegate is chosen dynamically depending on what
           groupRenderer is returned by the GroupsModel.

           Each groupRenderer should have a corresponding QML file with the
           same name that will be used as delegate.
           For example:

           If groupRenderer == 'UnityShowcaseRenderer' then it will look for
           the file 'UnityShowcaseRenderer.qml' and use it to render the group.
        */
        list.delegate: Loader {
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
            }

            onLoaded: {
                item.displayName = displayName
                item.iconHint = iconHint
                item.model = group_model
                item.place = place
            }
        }

        list.model: DeeListModel {
            service: dBusService
            objectPath: dBusDeePrefix + "GroupsModel"
        }
    }

}
