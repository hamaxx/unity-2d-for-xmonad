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
            /* FIXME: check for sections.model != undefined is a
               workaround for a crash in Qt happening when setting the model of
               a ListView to undefined after it being non-empty.
               It seems that the model count is not 0 and delegates are still
               being created and updated with data.

               To reproduce the crash: go to the applications place, exit the
               dash and then go to the home page of the dash.
            */
            label: sections.model != undefined ? column_0 : ""
            isActiveSection: dash.currentPage.model.activeSection == index

            onClicked: {
                sections.focus = false
                search_entry.focus = true
                dash.currentPage.model.activeSection = model.index
            }
        }

        model: dash.currentPage != undefined ? dash.currentPage.model.sections : undefined
    }
}
