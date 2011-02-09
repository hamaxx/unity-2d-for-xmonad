import Qt 4.7
import UnityApplications 1.0 /* Necessary for the ImageProvider serving image://icons/theme_name/icon_name */
import Unity2d 1.0 /* Necessary for QSortFilterProxyModelQML */
import Places 1.0 /* Necessary for DashDeclarativeView.*Dash */
import gconf 1.0

Item {
    property variant model: PageModel {
        /* model.entrySearchQuery is copied over to all place entries's globalSearchQuery property */
        onEntrySearchQueryChanged: {
            for (var i = 0; i < dash.places.rowCount(); i++) {
                dash.places.get(i).globalSearchQuery = entrySearchQuery
            }
            if (dashView.dashState == DashDeclarativeView.CollapsedDesktopDash) {
                dashView.dashState = DashDeclarativeView.ExpandedDesktopDash
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
            /* -2 is here because no rightMargin is set in ListViewWithScrollbar.list yet */
            width: ListView.view.width-2

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

        opacity: globalSearchActive || dashView.dashState == DashDeclarativeView.CollapsedDesktopDash ? 0 : 1
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.verticalCenter: parent.verticalCenter

        width: 692
        height: 348
        spacing: 62

        HomeButton {
            icon: "image://icons/unity-icon-theme/web"
            label: qsTr("Web")

            GConfItem {
                id: desktop_file_path
                key: "/desktop/gnome/applications/browser/exec"
            }

            onClicked: {
                dashView.active = false
                console.log("FIXME: should launch", desktop_file_path.value)
                Qt.openUrlExternally("http://")
            }
        }

        HomeButton {
            icon: "image://icons/unity-icon-theme/music"
            label: qsTr("Music")
            onClicked: activatePlaceEntry("/usr/share/unity/places/applications.place", "Files", 4)
        }

        HomeButton {
            icon: "image://icons/unity-icon-theme/photos"
            label: qsTr("Photos & Videos")
            onClicked: activatePlaceEntry("/usr/share/unity/places/applications.place", "Files", 4)
        }

        HomeButton {
            icon: "image://icons/unity-icon-theme/games"
            label: qsTr("Games")
            onClicked: activatePlaceEntry("/usr/share/unity/places/applications.place", "Files", 2)
        }

        HomeButton {
            icon: "image://icons/unity-icon-theme/email_and_chat"
            label: qsTr("Email & Chat")
            onClicked: activatePlaceEntry("/usr/share/unity/places/applications.place", "Files", 3)
        }

        HomeButton {
            icon: "image://icons/unity-icon-theme/work"
            label: qsTr("Office")
            onClicked: activatePlaceEntry("/usr/share/unity/places/applications.place", "Files", 5)
        }

        HomeButton {
            icon: "image://icons/unity-icon-theme/filesandfolders"
            label: qsTr("Files & Folders")
            onClicked: activatePlaceEntry("/usr/share/unity/places/files.place", "Files", 0)
        }

        HomeButton {
            icon: "image://icons/unity-icon-theme/softwarecentre"
            label: qsTr("Get New Apps")

            LauncherApplication {
                id: software_center
                desktop_file: "/usr/share/applications/ubuntu-software-center.desktop"
            }

            onClicked: {
                dashView.active = false
                software_center.activate()
            }
        }
    }
}
