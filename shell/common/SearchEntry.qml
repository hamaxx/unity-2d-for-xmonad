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
import Unity2d 1.0
import "fontUtils.js" as FontUtils
import "utils.js" as Utils

AbstractButton {
    property string searchQuery
    property string placeHolderText: ""
    property bool active: false

    signal activateFirstResult

    Accessible.name: "Search Entry"

    /* Delete search when set in-active */
    onActiveChanged: if (!active) searchInput.text = ""

    /* Keys forwarded to the search entry are forwarded to the text input. */
    Keys.forwardTo: [searchInput]

    opacity: ( state == "selected" || state == "hovered" ) ? 1.0 : 0.7

    BorderImage {
        anchors.fill: parent
        anchors.margins: -5
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
            id: searchIcon

            anchors.left: parent.left
            anchors.leftMargin: -9
            anchors.verticalCenter: parent.verticalCenter
            width: sourceSize.width
            height: sourceSize.height

            smooth: true

            source: searchInput.text ? "artwork/cross.png" : "artwork/search_icon.png"
            fillMode: Image.PreserveAspectFit
        }

        MouseArea {
            id: clearButton

            Accessible.name: "Clear"
            Accessible.role: Accessible.PushButton

            anchors.fill: searchIcon

            onClicked: {
                searchInput.forceActiveFocus()
                searchInput.text = ""
            }
        }

        TextInput {
            id: searchInput

            Accessible.name: searchInstructions.text
            Accessible.role: Accessible.EditableText

            anchors.left: searchIcon.right
            anchors.leftMargin: -5
            anchors.right: parent.right
            anchors.verticalCenter: parent.verticalCenter
            horizontalAlignment: Text.AlignLeft

            color: "#ffffff"
            font.pixelSize: FontUtils.fontSizeToPixels("xx-large")
            focus: true
            selectByMouse: true
            cursorDelegate: cursor
            selectionColor: "gray"

            onTextChanged: liveSearchTimeout.restart()

            Timer {
                id: liveSearchTimeout
                interval: 200
                onTriggered: searchQuery = searchInput.text
            }

            Keys.onPressed: {
                forceActiveFocus();
                if (event.key == Qt.Key_Return || event.key == Qt.Key_Enter) {
                    activateFirstResult()
                    event.accepted = true;
                }
                if (event.key == Qt.Key_Escape) {
                    if (searchInput.text != "") {
                        searchInput.text = ""
                        event.accepted = true;
                    }
                }
            }

            Component {
                id: cursor

                Rectangle {
                    property bool timerShowCursor: true

                    id: customCursor
                    color: "white"
                    anchors.top: parent.top
                    anchors.bottom: parent.bottom
                    anchors.topMargin: 2
                    anchors.bottomMargin: 2
                    width: 1
                    visible: parent.activeFocus && timerShowCursor
                    Timer {
                        interval: 800; running: customCursor.parent.activeFocus; repeat: true
                        onTriggered: {
                            interval = interval == 800 ? 400 : 800
                            customCursor.timerShowCursor = !customCursor.timerShowCursor
                        }
                    }
                }
            }

            TextCustom {
                id: searchInstructions

                anchors.left: parent.left
                anchors.right: parent.right
                elide: Text.ElideRight
                anchors.verticalCenter: parent.verticalCenter
                LayoutMirroring.enabled: false
                horizontalAlignment: Utils.isRightToLeft() ? Text.AlignRight : Text.AlignLeft

                color: "white"
                opacity: 0.5
                fontSize: "x-large"
                font.italic: true
                text: {
                    if(searchInput.text || searchInput.inputMethodComposing)
                        return ""
                    else
                        return placeHolderText
                }
            }
        }
    }

    CursorShapeArea {
        anchors.fill: parent
        cursorShape: Qt.IBeamCursor
    }
}
