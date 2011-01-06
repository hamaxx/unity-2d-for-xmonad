import Qt 4.7
import dee 1.0 /* Necessary for DeeListModel */
import UnityPlaces 1.0 /* Necessary for UnityPlaceEntry and UnityPlace */
import "utils.js" as Utils

QtObject {
    id: placeEntryModel

    /* === API === */

    /* Same as C++ */
    property string name
    property string icon
    property DeeListModel sections
    property DeeListModel entryResultsModel
    property DeeListModel entryGroupsModel
    property DeeListModel globalResultsModel
    property DeeListModel globalGroupsModel

    /* Not in C++ yet */
    /* The PlaceEntry D-Bus API does not expose properties but only setters for these.
       We make sure the setters are always synchronised with our local properties.
       See code in IMPLEMENTATION below. */
    property int activeSection
    property bool active: false
    property string searchQuery
    /* globalSearchQuery has the same value for all PlaceEntryModel */
    property string globalSearchQuery: dash.globalSearchQuery

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
            /* Override the files place's default URI handler: we want the file
               manager to handle opening folders, not the dash.

               Ref: https://bugs.launchpad.net/upicek/+bug/689667
             */
             Qt.openUrlExternally(decodeURIComponent(uri))
        }
        else if (!placeDBusInterface.Activate(uri)) {
            if (scheme == "application") {
                var path = matches[2]
                Utils.launchApplicationFromDesktopFile(path, placeEntryModel)
            }
            else {
                console.log("FIXME: Possibly no handler for scheme \'%1\'".arg(scheme))
                console.log("Trying to open", uri)
                /* Try our luck */
                Qt.openUrlExternally(decodeURIComponent(uri))
            }
        }
    }

    /* Will be deprecated by C++ */
    property string dBusService: "com.canonical.Unity." + name + "Place"
    property string dBusDeePrefix: "/com/canonical/dee/model/com/canonical/Unity/" + name + "Place/"
    property string dBusObjectPath
    property variant dBusInterface: UnityPlaceEntry {
                                         service: dBusService
                                         objectPath: dBusObjectPath
                                    }
    property string placeDBusObjectPath
    property variant placeDBusInterface: UnityPlace {
                                         service: dBusService
                                         objectPath: placeDBusObjectPath
                                    }


    /* === IMPLEMENTATION === */

    sections: DeeListModel {
                   service: dBusService
                   objectPath: dBusDeePrefix ? dBusDeePrefix + "SectionsModel" : ""
              }
    entryResultsModel: DeeListModel {
                                        service: dBusService
                                        objectPath: dBusDeePrefix ? dBusDeePrefix +"ResultsModel" : ""
                                   }
    entryGroupsModel: DeeListModel {
                                        service: dBusService
                                        objectPath: dBusDeePrefix ? dBusDeePrefix + "GroupsModel" : ""
                                   }
    globalResultsModel: DeeListModel {
                                        service: dBusService
                                        objectPath: dBusDeePrefix ? dBusDeePrefix + "GlobalResultsModel" : ""
                                   }

    /* Synchronise local properties with D-Bus setters */
    Component.onCompleted: {
        dBusInterface.SetActive(active)
        dBusInterface.SetActiveSection(activeSection)
        dBusInterface.SetSearch(searchQuery, [])
        dBusInterface.SetGlobalSearch(globalSearchQuery, [])
    }
    onActiveChanged: {
        dBusInterface.SetActive(active)
        /* PlaceEntry.SetActiveSection needs to be called after PlaceEntry.SetActive
           in order for it to have an effect. */
        if (active) {
            dBusInterface.SetActiveSection(activeSection)
        }
    }
    onActiveSectionChanged: dBusInterface.SetActiveSection(activeSection)
    onSearchQueryChanged: dBusInterface.SetSearch(searchQuery, [])
    onGlobalSearchQueryChanged: dBusInterface.SetGlobalSearch(globalSearchQuery, [])

}
