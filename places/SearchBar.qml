import Qt 4.7

FocusScope {
    /* Keys forwarded to the search bar are forwarded to the search entry. */
    Keys.forwardTo: [search_entry]

    BorderGlow {
        id: background

        anchors.fill: parent

        Image {
            anchors.fill: parent
            anchors.margins: 6
            source: "artwork/checker.png"
            fillMode: Image.Tile
        }
    }

    SearchEntry {
        id: search_entry

        focus: true
        KeyNavigation.right: sections

        width: 281
        anchors.left: parent.left
        anchors.leftMargin: 82
        anchors.top: parent.top
        anchors.topMargin: 7
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 6
    }

    ListView {
        id: sections

        KeyNavigation.left: search_entry

        visible: model != undefined
        orientation: ListView.Horizontal
        /* Non-draggable when all sections are visible */
        boundsBehavior: Flickable.StopAtBounds

        anchors.left: search_entry.right
        anchors.leftMargin: 14
        anchors.right: parent.right
        anchors.rightMargin: 15
        spacing: 10

        height: parent.height
        /* Make sure the first section is selected when getting the focus */
        currentIndex: 0
        onActiveFocusChanged: {
            /* It really should be onFocusChanged but the following bug makes
               using focus impossible:
               http://bugreports.qt.nokia.com/browse/QTBUG-12649
            */
            if(!focus) currentIndex = 0
        }

        delegate: Section {
            anchors.verticalCenter: parent.verticalCenter
            horizontalPadding: 4
            verticalPadding: 3
            label: column_0
            isActiveSection: current_page.activeSection == index

            onClicked: {
                sections.focus = false
                search_entry.focus = true
                current_page.setActiveSection(model.index)
            }
        }

        model: current_page.sections
    }
}
