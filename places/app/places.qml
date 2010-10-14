import Qt 4.7

Item {
    width: 800
    height: 600

    GnomeBackground {
        anchors.fill: parent
    }

    /* FIXME: the background needs to be darkened but compositing at
              rendering is way too expensive */
    Rectangle {
        anchors.fill: parent
        opacity: 0.4
        color: "black"
    }

    Place {
        anchors.fill: parent

        /* FIXME: these 2 properties need to be extracted from the place configuration file
                  located in /usr/share/unity/places/applications.place
        */
        name: "Applications"
        dBusObjectPath: "/com/canonical/unity/applicationsplace/applications"
    }
}
