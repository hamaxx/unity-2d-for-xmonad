import Qt 4.7
import UnityApplications 1.0 /* Necessary for LauncherPlacesList */
import Places 1.0 /* Necessary for DashDeclarativeView.*Dash */

Item {
    id: dash

    property variant currentPage

    Binding {
        target: dashView
        property: "expanded"
        value: (currentPage && currentPage.expanded != undefined) ? currentPage.expanded : true
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
        search_entry.focus = true
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
        pageLoader.item.shortcutsActive = false
        dashView.activePlaceEntry = ""
    }

    property variant places: LauncherPlacesList {
        Component.onCompleted: startAllPlaceServices()
    }

    GnomeBackground {
        anchors.fill: parent
        overlay_color: "black"
        overlay_alpha: 0.71
        visible: dashView.dashMode == DashDeclarativeView.FullScreenMode
    }

    BorderImage {
        anchors.fill: parent
        visible: dashView.dashMode == DashDeclarativeView.DesktopMode
        source: dashView.isCompositingManagerRunning ? "artwork/desktop_dash_background.sci" : "artwork/desktop_dash_background_no_transparency.sci"
    }

    Item {
        anchors.fill: parent
        visible: dashView.active

        /* Unhandled keys will always be forwarded to the search bar. That way
           the user can type and search from anywhere in the interface without
           necessarily focusing the search bar first. */
        Keys.forwardTo: [search_entry]


        SearchEntry {
            id: search_entry

            focus: true

            anchors.top: parent.top
            anchors.topMargin: 10
            anchors.left: parent.left
            anchors.leftMargin: 16
            anchors.right: refine_search.left
            anchors.rightMargin: 10

            height: 53
        }

        SearchRefine {
            id: refine_search

            /* SearchRefine is only to be displayed for places, not in the home page */
            visible: dashView.activePlaceEntry != ""
            placeEntryModel: visible && currentPage != undefined ? currentPage.model : undefined

            anchors.top: search_entry.anchors.top
            anchors.topMargin: search_entry.anchors.topMargin
            height: parent.height
            headerHeight: search_entry.height
            width: 295
            anchors.right: parent.right
            anchors.rightMargin: 19
        }

        Button {
            id: fullScreenButton
            anchors.bottom: parent.bottom
            anchors.right: parent.right
            anchors.rightMargin: 15
            anchors.bottomMargin: 15
            width: fullScreenButtonImage.sourceSize.width
            height: fullScreenButtonImage.sourceSize.height
            visible: dashView.dashMode != DashDeclarativeView.FullScreenMode

            Image {
                id: fullScreenButtonImage
                source: "artwork/fullscreen_button.png"
            }

            onClicked: {
                dashView.dashMode = DashDeclarativeView.FullScreenMode
            }
        }

        Loader {
            id: pageLoader

            anchors.top: search_entry.bottom
            anchors.topMargin: 2
            anchors.bottom: parent.bottom
            anchors.bottomMargin: 12
            anchors.left: parent.left
            anchors.leftMargin: 20
            anchors.right: refine_search.folded ? parent.right : refine_search.left
            anchors.rightMargin: refine_search.folded ? 0 : 15
        }
    }
}
