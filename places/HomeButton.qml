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

Button {
    property alias icon: icon.source
    property alias label: label.text
    property alias iconSourceSize: icon.sourceSize

    width: 160
    height: 172

    Item {
        anchors.right: parent.right
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.bottom: label.bottom
        anchors.bottomMargin: 30

        Image {
            id: icon

            width: sourceSize.width
            height: sourceSize.height

            anchors.centerIn: parent
            fillMode: Image.PreserveAspectFit

            asynchronous: true
            opacity: status == Image.Ready ? 1 : 0
            Behavior on opacity {NumberAnimation {duration: 200; easing.type: Easing.InOutQuad}}
        }
    }

    TextMultiLine {
        id: label

        color: parent.state == "pressed" ? "#444444" : "#ffffff"
        horizontalAlignment: Text.AlignHCenter
        anchors.bottom: parent.bottom
        anchors.right: parent.right
        anchors.left: parent.left
        anchors.bottomMargin: 3
        anchors.rightMargin: 5
        anchors.leftMargin: 7
        height: 40
        font.underline: parent.activeFocus
        font.bold: true
        font.pixelSize: 16
    }
}
