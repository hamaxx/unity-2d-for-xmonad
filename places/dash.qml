import Qt 4.7
import UnityApplications 1.0 /* Necessary for LauncherPlacesList */

Item {
    id: dash

    /* globalSearchQuery is used to store the PlaceEntryModel.globalSearchQuery string
       common to all the PlaceEntryModel components */
    property string globalSearchQuery
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

        placeEntryModel.activeSection = section
        placeEntryModel.active = true
        placeEntryView.model = placeEntryModel
        activatePage(placeEntryView)
    }

    function activateHome() {
        activatePage(home)
    }

    property variant places: LauncherPlacesList {}

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

        Item {
            id: pages

            anchors.top: search_bar.bottom
            anchors.topMargin: 12
            anchors.bottom: parent.bottom
            anchors.bottomMargin: 12
            anchors.left: parent.left
            anchors.leftMargin: 7
            anchors.right: parent.right
            anchors.rightMargin: 8

            Home {
                id: home
                anchors.fill: parent
                visible: false
            }

            PlaceEntryView {
                id: placeEntryView
                anchors.fill: parent
                visible: false
            }
        }
    }
}
