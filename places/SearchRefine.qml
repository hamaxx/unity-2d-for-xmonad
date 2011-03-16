import Qt 4.7
import Effects 1.0

FocusScope {
    id: searchRefine

    property bool folded: true
    property int headerHeight
    property variant placeEntryModel

    AbstractButton {
        id: header

        effect: DropShadow {
                    blurRadius: {
                        switch (header.state) {
                            case "selected":
                                return 4
                            case "pressed":
                                return 8
                            default:
                                return 0
                        }
                    }
                    offset.x: 0
                    offset.y: 0
                    color: "white"
                }

        onClicked: searchRefine.folded = !searchRefine.folded

        focus: true

        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.topMargin: 10
        height: headerHeight - anchors.topMargin

        TextCustom {
            id: title

            text: u2d.tr("Refine search")
            font.bold: true
            font.pixelSize: 16

            anchors.top: parent.top
            anchors.left: parent.left
        }

        FoldingArrow {
            id: arrow

            folded: searchRefine.folded

            anchors.verticalCenter: title.verticalCenter
            anchors.left: title.right
            anchors.leftMargin: 10
        }
    }

    VisualItemModel {
        id: optionsModel

        SearchRefineOptionType {
            title: u2d.tr("Type")
            placeEntryModel: searchRefine.placeEntryModel
        }
    }

    ListView {
        id: options

        opacity: folded ? 0.0 : 1.0
        Behavior on opacity {NumberAnimation {duration: 100; easing.type: Easing.InOutQuad}}

        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: header.bottom
        anchors.topMargin: 7
        anchors.bottom: parent.bottom
        orientation: ListView.Vertical

        /* Make sure the first section is selected when getting the focus */
        currentIndex: 0
        onActiveFocusChanged: {
            /* It really should be onFocusChanged but the following bug makes
               using focus impossible (fixed in Qt 4.7.1):
               http://bugreports.qt.nokia.com/browse/QTBUG-12649
            */
            if(!focus) currentIndex = 0
        }

        /* Non-draggable when all items are visible */
        boundsBehavior: Flickable.StopAtBounds

        model: optionsModel
    }
}
