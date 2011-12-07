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

import QtQuick 1.0

AbstractButton {
    id: button

    property alias icon: icon.source
    property alias label: label.text
    property alias iconSourceSize: icon.sourceSize

    Accessible.name: label.text

    width: 160
    height: 172

    ButtonBackground {
        anchors.fill: icon
        anchors.margins: -5
        state: button.state
    }

    Image {
        id: icon

        width: sourceSize.width
        height: sourceSize.height

        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: parent.top
        anchors.topMargin: 5
        fillMode: Image.PreserveAspectFit

        asynchronous: true
        opacity: status == Image.Ready ? 1 : 0
        Behavior on opacity {NumberAnimation {duration: 200; easing.type: Easing.InOutQuad}}
    }

    TextMultiLine {
        id: label

        color: "#ffffff"
        state: ( parent.state == "selected" || parent.state == "hovered" ) ? "expanded" : ""
        horizontalAlignment: Text.AlignHCenter
        anchors.top: icon.bottom
        anchors.right: parent.right
        anchors.left: parent.left
        anchors.topMargin: 8
        anchors.rightMargin: 5
        anchors.leftMargin: 7
        height: 40
        font.bold: true
        fontSize: "large"
    }
}
