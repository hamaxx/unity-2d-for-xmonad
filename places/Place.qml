import Qt 4.7
import QtDee 1.0
import UnityApplications 1.0 /* Necessary for the ImageProvider serving image://icons */
import UnityPlaces 1.0
import "utils.js" as Utils

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
           group_renderer is returned by the GroupsModel.

           Each group_renderer should have a corresponding QML file with the
           same name that will be used as delegate.
           For example:

           If group_renderer == 'UnityShowcaseRenderer' then it will look for
           the file 'UnityShowcaseRenderer.qml' and use it to render the group.
        */
        delegate: Loader {
            property string group_renderer: column_0
            property string display_name: column_1
            property string icon_hint: column_2
            property int group_id: index

            source: group_renderer ? group_renderer+".qml" : ""
            onStatusChanged: {
                if (status == Loader.Error)
                    console.log("Failed to load renderer", group_renderer)
            }

            /* -2 is here to prevent clipping of the group; it looks like a bug */
            width: ListView.view.width-2

            /* Model that will be used by the group's delegate */
            QSortFilterProxyModelQML {
                id: group_model

                model: resultsModel

                /* resultsModel contains data for all the groups of a given Place.
                   Each row has a column (the second one) containing the id of
                   the group it belongs to (group_id).
                */
                filterRole: 2 /* second column (see above comment) */
                filterRegExp: RegExp(group_id)

                /* Maximum number of items in the model; -1 is unlimited */
                limit: item.model_count_limit
            }

            onLoaded: {
                item.display_name = display_name
                item.icon_hint = icon_hint
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
