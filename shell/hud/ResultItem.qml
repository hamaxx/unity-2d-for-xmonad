/*
* This file is part of unity-2d
*
* Copyright 2012 Canonical Ltd.
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
import "../common"

AbstractButton {
    id: delegate
    property string icon: ""

    Accessible.name: plainText
    Accessible.role: Accessible.Button

    Rectangle {
        id: line

        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        height: 1

        color: "#21ffffff" // 80% opaque
    }

    Rectangle {
        id: container
        objectName: "container"

        anchors.top: line.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom

        border.color: "white"
        border.width: (parent.state == "selected" || parent.state == "hovered"
                       || parent.state == "pressed") ? 2 : 0
        color: {
            if (parent.state == "selected" || parent.state == "hovered") return "#21ffffff"
            else if ( parent.state == "pressed" ) return "white"
            else return "transparent"
        }
        radius: 7
        smooth: true

        TextCustom {
            id: resultText
            objectName: "result"

            anchors.left: parent.left
            anchors.right: shortcutText.left
            anchors.leftMargin: 38
            anchors.rightMargin: 4
            anchors.verticalCenter: container.verticalCenter

            color: (delegate.state == "pressed") ? "black" : "white"
            fontSize: "large"
            font.weight: Font.Light
            maximumLineCount: 1
            clip: true

            textFormat: Text.RichText
            text: formattedText
        }

        TextCustom {
            id: shortcutText
            objectName: "shortcut"

            width: paintedWidth
            anchors.right: parent.right
            anchors.rightMargin: 16
            anchors.verticalCenter: container.verticalCenter

            color: (delegate.state == "pressed") ? "black" : "white"
            fontSize: "small"
            font.weight: Font.Light

            text: shortcut
        }
    }
}
