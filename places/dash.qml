import Qt 4.7

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

    function findPlaceEntryModel(fileName, groupName) {
        var place
        for (var i = 0; i < places.length; i++) {
            place = places[i]
            if (place.fileName == fileName && place.groupName == groupName)
                return place
        }
        return null
    }

    function activatePlaceEntry(fileName, groupName, section) {
        var placeEntryModel = findPlaceEntryModel(fileName, groupName)
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


    /* FIXME: hardcoded list of places
              Ref: https://bugs.launchpad.net/bugs/684152 */
    property variant places: [files_place, applications_place]

    PlaceEntryModel {
        id: applications_place

        fileName: "/usr/share/unity/places/applications.place"
        groupName: "Files"

        /* FIXME: these 2 properties need to be extracted from the place configuration file
                  located in /usr/share/unity/places/applications.place
        */
        name: "Applications"
        placeDBusObjectPath: "/com/canonical/unity/applicationsplace"
        dBusObjectPath: placeDBusObjectPath+"/applications"
        icon: "/usr/share/unity/applications.png"
    }

    PlaceEntryModel {
        id: files_place

        fileName: "/usr/share/unity/places/files.place"
        groupName: "Files"

        /* FIXME: these 2 properties need to be extracted from the place configuration file
                  located in /usr/share/unity/places/files.place
        */
        name: "Files"
        placeDBusObjectPath: "/com/canonical/unity/filesplace"
        dBusObjectPath: placeDBusObjectPath+"/files"
        icon: "/usr/share/unity/files.png"
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
