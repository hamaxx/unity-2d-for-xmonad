/*
 * This file is part of unity-2d
 *
 * Copyright 2010-2011 Canonical Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 3.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

import QtQuick 1.1
import Effects 1.0

AbstractButton {
    property string searchQuery

    Accessible.name: "Search Entry"

    /* Cancels current search when the dash becomes invisible */
    Connections {
        target: dashView
        onActiveChanged: if (!dashView.active) search_input.text = ""
    }

    Binding {
        target: dash.currentPage != undefined ? dash.currentPage.model : null
        property: "searchQuery"
        value: searchQuery
    }

    /* Keys forwarded to the search entry are forwarded to the text input. */
    Keys.forwardTo: [search_input]

    opacity: ( state == "selected" || state == "hovered" ) ? 1.0 : 0.7

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

            Accessible.name: "Clear"
            Accessible.role: Accessible.PushButton

            anchors.fill: search_icon

            onClicked: {
                search_input.forceActiveFocus()
                search_input.text = ""
            }
        }

        TextInput {
            id: search_input

            Accessible.name: search_instructions.text
            Accessible.role: Accessible.EditableText

            effect: DropShadow {
                    id: glow

                    blurRadius: 4
                    offset.x: 0
                    offset.y: 0
                    color: "white"
                    enabled: search_input.text != "" || search_input.inputMethodComposing
                }

            anchors.left: search_icon.right
            anchors.leftMargin: -5
            anchors.right: parent.right
            anchors.verticalCenter: parent.verticalCenter
            horizontalAlignment: Text.AlignLeft

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
                if (event.key == Qt.Key_Return || event.key == Qt.Key_Enter) {
                    dash.currentPage.activateFirstResult()
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

            TextCustom {
                id: search_instructions

                anchors.left: parent.left
                anchors.right: parent.right
                anchors.verticalCenter: parent.verticalCenter
                LayoutMirroring.enabled: false
                horizontalAlignment: isRightToLeft() ? Text.AlignRight : Text.AlignLeft

                color: "white"
                opacity: 0.5
                fontSize: "x-large"
                font.italic: true
                text: {
                    if(search_input.text || search_input.inputMethodComposing)
                        return ""
                    else if(dash.currentPage != undefined && dash.currentPage.model.searchHint)
                        return dash.currentPage.model.searchHint
                    else
                        return u2d.tr("Search")
                }
            }
        }
    }
}
