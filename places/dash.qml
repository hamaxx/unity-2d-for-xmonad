import Qt 4.7

Item {
    width: 965
    height: 575

    property variant current_page: home

    function activatePage(page) {
        current_page.visible = false
        current_page = page
        current_page.visible = true
    }

    function activatePlace(place, section) {
        place.setActiveSection(section)
        activatePage(place)
    }

    GnomeBackground {
        anchors.fill: parent
    }

    /* FIXME: the background needs to be darkened but compositing at
              rendering is way too expensive

              Use QGraphicsView::setBackgroundBrush with
              QGraphicsView::setCacheMode instead.
    */
    Item {
        anchors.fill: parent
        visible: dashView.active

        Rectangle {
            anchors.fill: parent
            opacity: 0.37
            color: "black"
        }

        SearchBar {
            id: search_bar

            anchors.top: parent.top
            anchors.left: parent.left
            anchors.leftMargin: 2
            anchors.right: parent.right
            anchors.rightMargin: 3
            height: 45
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
            }

            Place {
                id: applications_place

                visible: false
                anchors.fill: parent

                /* FIXME: these 2 properties need to be extracted from the place configuration file
                          located in /usr/share/unity/places/applications.place
                */
                name: "Applications"
                dBusObjectPath: "/com/canonical/unity/applicationsplace/applications"
            }

            Place {
                id: files_place

                visible: false
                anchors.fill: parent

                /* FIXME: these 2 properties need to be extracted from the place configuration file
                          located in /usr/share/unity/places/files.place
                */
                name: "Files"
                dBusObjectPath: "/com/canonical/unity/filesplace/files"
            }
        }
    }
}
