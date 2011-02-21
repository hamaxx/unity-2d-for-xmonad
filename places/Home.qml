import Qt 4.7
import UnityApplications 1.0 /* Necessary for the ImageProvider serving image://icons/theme_name/icon_name */
import Unity2d 1.0 /* Necessary for QSortFilterProxyModelQML */
import gconf 1.0

Item {
    property variant model: PageModel {
        /* model.entrySearchQuery is copied over to all place entries's globalSearchQuery property */
        onEntrySearchQueryChanged: {
            for (var i = 0; i < dash.places.rowCount(); i++) {
                dash.places.get(i).globalSearchQuery = entrySearchQuery
            }
        }
    }

    /* Either globalSearch is shown or buttons are shown depending on globalSearchActive */
    property bool globalSearchActive: model.entrySearchQuery != ""

    ListViewWithScrollbar {
        id: globalSearch

        opacity: globalSearchActive ? 1 : 0
        anchors.fill: parent

        list.model: dash.places

        list.delegate: UnityDefaultRenderer {
            width: ListView.view.width

            parentListView: list
            placeEntryModel: modelData
            displayName: modelData.name
            iconHint: modelData.icon

            /* Filter out results for which the corresponding group's renderer
               is 'UnityEmptySearchRenderer'.
               Each result has a column (the second one) containing the id of
               the group it belongs to (groupId).
            */
            model:  QSortFilterProxyModelQML {
                model: modelData.globalResultsModel

                /* FIXME: we ignore the groupId with renderer 'UnityEmptySearchRenderer'
                   by hardcoding it instead of looking it up in the Place's
                   groupsModel as Unity does.

                   Two solutions could be envisioned:
                   1) Actually looking for the row in the Place's groupsModel
                      that has in its first column 'UnityEmptySearchRenderer'.
                      That would require adding an API in libqtdee's DeeListModel.
                   2) Changing the behaviour of the place daemons so that the
                      Place's globalResultsModel is empty when there are no
                      results. The applications place does that but not the
                      files place.
                */
                property int ignoredGroupId: 5
                filterRole: 2 /* groupId column */
                filterRegExp: RegExp("^[^%1]$".arg(ignoredGroupId)) /* anything but the ignoredGroupId */
            }
        }
    }

    Flow {
        id: buttons

        opacity: globalSearchActive ? 0 : 1
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.verticalCenter: parent.verticalCenter

        width: 868
        height: 420
        spacing: 65

        /* FIXME: dummy icons need to be replaced by design's */
        HomeButton {
            label: qsTr("Find Media Apps")
            icon: "image://icons/applications-multimedia"
            onClicked: activatePlaceEntry("/usr/share/unity/places/applications.place", "Files", 4)
        }

        HomeButton {
            label: qsTr("Find Internet Apps")
            icon: "image://icons/applications-internet"
            onClicked: activatePlaceEntry("/usr/share/unity/places/applications.place", "Files", 3)
        }

        HomeButton {
            label: qsTr("Find More Apps")
            icon: "image://icons/find"
            onClicked: activatePlaceEntry("/usr/share/unity/places/applications.place", "Files", 0)
        }

        HomeButton {
            label: qsTr("Find Files")
            icon: "image://icons/folder-saved-search"
            onClicked: activatePlaceEntry("/usr/share/unity/places/files.place", "Files", 0)
        }

        /* FIXME: use user's preferred applications instead of hardcoding them */
        HomeButtonApplication {
            label: qsTr("Browse the Web")
            desktopFile: "firefox.desktop"
        }

        HomeButtonApplication {
            label: qsTr("View Photos")
            desktopFile: "shotwell.desktop"
        }

        HomeButtonApplication {
            label: qsTr("Check Email")
            desktopFile: "evolution.desktop"
        }

        HomeButtonApplication {
            label: qsTr("Listen to Music")
            desktopFile: "banshee-1.desktop"
        }
    }
}
