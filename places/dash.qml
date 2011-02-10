import Qt 4.7
import UnityApplications 1.0 /* Necessary for LauncherPlacesList */
import Places 1.0 /* Necessary for DashDeclarativeView.*Dash */

Item {
    id: dash

    property variant currentPage

    states: [
        State {
            name: "collapsed"
            when: !currentPage.expanded
            StateChangeScript {
                script: updateDashState()
            }
        },
        State {
            name: "expanded"
            when: currentPage.expanded
            StateChangeScript {
                script: updateDashState()
            }
        }
    ]

    function updateDashState() {
        if (dashView.dashState == DashDeclarativeView.FullScreenDash) {
            return;
        }
        dashView.dashState = currentPage.expanded
            ? DashDeclarativeView.ExpandedDesktopDash
            : DashDeclarativeView.CollapsedDesktopDash
    }

    function activatePage(page) {
        if (page == currentPage) {
            return
        }

        if (currentPage != undefined) {
            currentPage.visible = false
        }
        currentPage = page
        currentPage.visible = true
        /* FIXME: For some reason currentPage gets the focus when it becomes
           visible. Reset the focus to the search_bar instead.
           It could be due to Qt bug QTBUG-13380:
           "Listview gets focus when it becomes visible"
        */
        search_bar.focus = true
    }

    function activatePlaceEntry(fileName, groupName, section) {
        var placeEntryModel = places.findPlaceEntry(fileName, groupName)
        if (placeEntryModel == null) {
            console.log("No match for place: %1 [Entry:%2]".arg(fileName).arg(groupName))
            return
        }

        /* FIXME: PlaceEntry.SetActiveSection needs to be called after
           PlaceEntry.SetActive in order for it to have an effect.
           This is likely a bug in the place daemons.
        */
        placeEntryModel.active = true
        placeEntryModel.activeSection = section
        pageLoader.source = "PlaceEntryView.qml"
        /* Take advantage of the fact that the loaded qml is local and setting
           the source loads it immediately making pageLoader.item valid */
        pageLoader.item.model = placeEntryModel
        activatePage(pageLoader.item)
        dashView.activePlaceEntry = placeEntryModel.dbusObjectPath
    }

    function activateHome() {
        pageLoader.source = "Home.qml"
        /* Take advantage of the fact that the loaded qml is local and setting
           the source loads it immediately making pageLoader.item valid */
        activatePage(pageLoader.item)
        dashView.activePlaceEntry = ""
    }

    property variant places: LauncherPlacesList {
        Component.onCompleted: startAllPlaceServices()
    }

    GnomeBackground {
        anchors.fill: parent
        overlay_color: "black"
        overlay_alpha: 0.71
        visible: dashView.dashState == DashDeclarativeView.FullScreenDash
    }

    BorderImage {
        anchors.fill: parent
        visible: dashView.dashState == DashDeclarativeView.CollapsedDesktopDash || dashView.dashState == DashDeclarativeView.ExpandedDesktopDash
        source: "artwork/desktop_dash.sci"
    }

    Item {
        anchors.fill: parent
        visible: dashView.active

        /* Unhandled keys will always be forwarded to the search bar. That way
           the user can type and search from anywhere in the interface without
           necessarily focusing the search bar first. */
        Keys.forwardTo: [search_bar]

        SearchBar {
            id: search_bar

            focus: true

            anchors.top: parent.top
            anchors.left: parent.left
            anchors.leftMargin: 3
            width: 616
            height: 47
        }

        Button {
            id: shortcutsButton
            anchors.top: parent.top
            anchors.right: parent.right
            anchors.rightMargin: 44
            width: 80
            height: 47

            TextCustom {
                text: "Shortcuts"
                anchors.fill: parent
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
            }

            opacity: dashView.dashState == DashDeclarativeView.CollapsedDesktopDash ? 1 : 0

            onClicked: {
                dashView.dashState = DashDeclarativeView.ExpandedDesktopDash
            }
        }

        MouseArea {
            id: fullScreenButton
            anchors.bottom: parent.bottom
            anchors.right: parent.right
            width: 40
            height: 40

            onClicked: {
                dashView.dashState = DashDeclarativeView.FullScreenDash
            }
        }

        Loader {
            id: pageLoader

            anchors.top: search_bar.bottom
            anchors.topMargin: 12
            anchors.bottom: parent.bottom
            anchors.bottomMargin: 12
            anchors.left: parent.left
            anchors.leftMargin: 7
            anchors.right: parent.right
            anchors.rightMargin: 8
        }
    }
}
