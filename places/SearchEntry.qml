import Qt 4.7
import Effects 1.0

FocusScope {
    property string searchQuery

    /* Cancels current search when the dash becomes invisible */
    Connections {
        target: dashView
        onActiveChanged: if(!dashView.active) search_input.text = ""
    }

    Binding {
        target: dash.currentPage != undefined ? dash.currentPage.model : null
        property: "entrySearchQuery"
        value: searchQuery
    }

    /* Keys forwarded to the search entry are forwarded to the text input. */
    Keys.forwardTo: [search_input]

    BorderImage {
        anchors.fill: parent
        source: "artwork/search_background.sci"
        smooth: false
    }

    Item {
        anchors.fill: parent
        anchors.topMargin: 6
        anchors.bottomMargin: 6
        anchors.leftMargin: 6
        anchors.rightMargin: 16

        Image {
            id: search_icon

            anchors.left: parent.left
            anchors.leftMargin: -5
            anchors.top: parent.top
            anchors.bottom: parent.bottom
            width: sourceSize.width

            smooth: true

            source: search_input.text ? "artwork/cross.png" : "artwork/search_icon.png"
            fillMode: Image.PreserveAspectFit
        }

        MouseArea {
            id: clear_button

            anchors.fill: search_icon

            onClicked: {
                search_input.forceActiveFocus()
                search_input.text = ""
            }
        }

        TextInput {
            id: search_input

            effect: DropShadow {
                    id: glow

                    blurRadius: 4
                    offset.x: 0
                    offset.y: 0
                    color: "white"
                    enabled: search_input.text != ""
                }

            anchors.left: search_icon.right
            anchors.leftMargin: -5
            anchors.right: parent.right
            anchors.verticalCenter: parent.verticalCenter

            color: "#ffffff"
            font.pixelSize: 28
            focus: true
            selectByMouse: true
            cursorDelegate: cursor

            onTextChanged: live_search_timeout.restart()

            Timer {
                id: live_search_timeout
                interval: 200
                onTriggered: searchQuery = search_input.text
            }

            Keys.onPressed: {
                if (event.key == Qt.Key_Return) {
                    searchQuery = text
                    event.accepted = true;
                }
            }

            Component {
                id: cursor

                Rectangle {
                    color: "white"
                    width: 2
                    height: 16

                    /* WARNING: that animation uses resources */
                    /* The following animation would behave exactly like
                       Unity if only 'search_input' could be referenced from
                       within the cursor Component.
                    /*
                    SequentialAnimation on opacity {
                        id: cursor_pulse
                        loops: 30
                        running: false
                        PropertyAnimation { duration: 1000; to: 0; easing.type: Easing.InOutQuad }
                        PropertyAnimation { duration: 1000; to: 1; easing.type: Easing.InOutQuad }
                    }
                    Connections {
                        target: search_input
                        onTextChanged: cursor_pulse.running = true
                        onActiveFocusChanged: cursor_pulse.running = search_input.activeFocus
                    }*/
                }
            }

            Text {
                id: search_instructions

                anchors.left: parent.left
                anchors.right: parent.right
                anchors.verticalCenter: parent.verticalCenter

                color: "white"
                opacity: 0.5
                font.pixelSize: 20
                font.italic: true
                text: {
                    if(search_input.text)
                        return ""
                    else if(dash.currentPage != undefined && dash.currentPage.model.name)
                        return qsTr("Search for %1").arg(dash.currentPage.model.name)
                    else
                        return qsTr("Search")
                }
            }
        }
    }
}
