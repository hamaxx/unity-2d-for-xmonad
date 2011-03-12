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
        }
    }

    /* Set to true if shortcut buttons are visible */
    property bool shortcutsActive: false

    /* Either globalSearch is shown or buttons are shown depending on globalSearchActive */
    property bool globalSearchActive: model.entrySearchQuery != ""
    
    /* Used by dash.qml to bind to dashView "expanded" property */
    property bool expanded: globalSearchActive || shortcutsActive

    Button {
        id: openShortcutsButton

        anchors.bottom: parent.top
        anchors.right: parent.right
        anchors.rightMargin: 50
        anchors.bottomMargin: 10
        width: childrenRect.width
        height: childrenRect.height

        Image {
            id: icon
            source: "artwork/open_shortcuts.png"
            width: sourceSize.width
            height: sourceSize.height
            anchors.left: parent.left
        }

        TextCustom {
            text: "Shortcuts"
            anchors.left: icon.right
            anchors.leftMargin: 3
            width: paintedWidth
            height: icon.height
            font.pixelSize: 16
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
        }

        opacity: (!expanded && dashView.dashMode == DashDeclarativeView.DesktopMode) ? 1 : 0
        Behavior on opacity {NumberAnimation {duration: 100}}

        onClicked: {
            shortcutsActive = true
        }
    }

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

    Rectangle {
        id: shortcuts

        opacity: (!globalSearchActive && (shortcutsActive || dashView.dashMode == DashDeclarativeView.FullScreenMode)) ? 1 : 0
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.verticalCenter: parent.verticalCenter

        width: 888
        height: 466

        radius: 5
        border.width: 1
        /* FIXME: wrong colors */
        border.color: Qt.rgba(1, 1, 1, 0.2)
        color: Qt.rgba(0, 0, 0, 0.3)

        Button {
            id: closeShortcutsButton

            anchors.left: parent.left
            anchors.top: parent.top
            anchors.leftMargin: -width/2

            width: childrenRect.width
            height: childrenRect.height

            Image {
                id: search_icon

                width: sourceSize.width
                height: sourceSize.height

                source: "artwork/cross.png"
            }

            opacity: (expanded && dashView.dashMode == DashDeclarativeView.DesktopMode) ? 1 : 0
            Behavior on opacity {NumberAnimation {duration: 100}}

            onClicked: shortcutsActive = false
        }

        Flow {
            anchors.fill: parent
            anchors.topMargin: 26
            anchors.bottomMargin: 35
            anchors.leftMargin: 32
            anchors.rightMargin: 32
            spacing: 61

            /* FIXME: dummy icons need to be replaced by design's */
            HomeButton {
                label: qsTr("Find Media Apps")
                icon: "artwork/find_media_apps.png"
                onClicked: activatePlaceEntry("/usr/share/unity/places/applications.place", "Files", 9)
            }

            HomeButton {
                label: qsTr("Find Internet Apps")
                icon: "artwork/find_internet_apps.png"
                onClicked: activatePlaceEntry("/usr/share/unity/places/applications.place", "Files", 8)
            }

            HomeButton {
                label: qsTr("Find More Apps")
                icon: "artwork/find_more_apps.png"
                onClicked: activatePlaceEntry("/usr/share/unity/places/applications.place", "Files", 0)
            }

            HomeButton {
                label: qsTr("Find Files")
                icon: "artwork/find_files.png"
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
}
