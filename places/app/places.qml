import Qt 4.7

Rectangle {
    id: place

    width: 800
    height: 600
    color: "grey"

    Place {
        anchors.fill: parent

        /* FIXME: these 2 properties need to be extracted from the place configuration file
                  located in /usr/share/unity/places/applications.place
        */
        name: "Applications"
        dBusObjectPath: "/com/canonical/unity/applicationsplace/applications"
    }
}
