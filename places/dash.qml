import Qt 4.7
import UnityApplications 1.0 /* Necessary for LauncherPlacesList */

Item {
    id: dash

    property variant currentPage

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
    }

    function activateHome() {
        pageLoader.source = "Home.qml"
        /* Take advantage of the fact that the loaded qml is local and setting
           the source loads it immediately making pageLoader.item valid */
        activatePage(pageLoader.item)
    }

    property variant places: LauncherPlacesList {
        Component.onCompleted: startAllPlaceServices()
    }

    GnomeBackground {
        anchors.fill: parent
        overlay_color: "black"
        overlay_alpha: 0.71
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
            anchors.right: parent.right
            anchors.rightMargin: 4
            height: 47
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
