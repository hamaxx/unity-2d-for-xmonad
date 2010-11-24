import Qt 4.7

FocusScope {
    /* Keys forwarded to the search entry are forwarded to the text input. */
    Keys.forwardTo: [search_input]

    BorderGlow {
        anchors.fill: parent
    }

    Item {
        anchors.fill: parent
        anchors.topMargin: 6
        anchors.bottomMargin: 6
        anchors.leftMargin: 6
        anchors.rightMargin: 6

        Image {
            id: search_icon

            anchors.left: parent.left
            anchors.leftMargin: 3
            anchors.top: parent.top
            anchors.topMargin: 1
            anchors.bottom: parent.bottom
            anchors.bottomMargin: 1
            smooth: true
            width: 18

            source: "/usr/share/unity/search_icon.png"
            fillMode: Image.PreserveAspectCrop
        }

        TextInput {
            id: search_input

            anchors.left: search_icon.right
            anchors.leftMargin: 2
            anchors.right: clear_button.left
            anchors.rightMargin: 5
            anchors.bottom: parent.bottom
            anchors.bottomMargin: 3

            color: "#ffffff"
            focus: true
            selectByMouse: true
            cursorDelegate: cursor

            onTextChanged: live_search_timeout.restart()

            Timer {
                id: live_search_timeout
                interval: 200
                onTriggered: current_page.search(search_input.text)
            }

            Keys.onPressed: {
                if (event.key == Qt.Key_Return) {
                    current_page.search(text)
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

                anchors.fill: parent
                anchors.leftMargin: 4

                font.italic: true
                color: "#eeeeee"
                text: {
                    if(search_input.text)
                        return ""
                    else if(current_page.name)
                        return qsTr("Search %1").arg(current_page.name)
                    else
                        return qsTr("Search")
                }
            }
        }

        Image {
            id: clear_button

            anchors.right: parent.right
            anchors.rightMargin: 4
            anchors.verticalCenter: parent.verticalCenter
            width: 10

            source: "artwork/cross.png"
            fillMode: Image.PreserveAspectFit

            MouseArea {
                anchors.fill: parent
                onClicked: {
                    search_input.forceActiveFocus()
                    search_input.text = ""
                }
            }
        }
    }
}
