import Qt 4.7

Item {
    width: 966
    height: 575

    property variant current_page

    function activatePage(page) {
        if (current_page != undefined)
            current_page.visible = false
        current_page = page
        current_page.visible = true
        /* FIXME: For some reason current_page gets the focus when it becomes
           visible. Reset the focus to the search_bar instead.
           It could be due to Qt bug QTBUG-13380:
           "Listview gets focus when it becomes visible"
        */
        search_bar.focus = true
        console.log(search_bar.search_entry.search_input)
    }

    function activatePlace(place, section) {
        place.setActiveSection(section)
        activatePage(place)
    }

    function activateHome() {
        activatePage(home)
    }

    GnomeBackground {
        anchors.fill: parent
        overlay_color: "black"
        overlay_alpha: dashView.active ? 0.37 : 0
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

            Place {
                id: applications_place
                objectName: "applications_place"

                visible: false
                anchors.fill: parent

                /* FIXME: these 2 properties need to be extracted from the place configuration file
                          located in /usr/share/unity/places/applications.place
                */
                name: "Applications"
                dBusObjectPath: "/com/canonical/unity/applicationsplace"
                dBusObjectPathPlaceEntry: dBusObjectPath+"/applications"
            }

            Place {
                id: files_place
                objectName: "files_place"

                visible: false
                anchors.fill: parent

                /* FIXME: these 2 properties need to be extracted from the place configuration file
                          located in /usr/share/unity/places/files.place
                */
                name: "Files"
                dBusObjectPath: "/com/canonical/unity/filesplace"
                dBusObjectPathPlaceEntry: dBusObjectPath+"/files"
            }
        }
    }
}
