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
    id: multiRangeButton

    property string text
    property bool checked: false
    property bool canUncheck: true
    property bool isLast: false

    focus: true

    Accessible.name: text
    Accessible.role: Accessible.Slider

    TextCustom {
        id: label
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        width: parent.width-1
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        font.pixelSize: (multiRangeButton.state == "selected") ? 10 : 12
        color: (checked) ? "red" : "white"
        text: multiRangeButton.text
        elide: Text.ElideRight
        opacity: ( !canUncheck ) ? 0 : 1
    }

    Rectangle {
        id: separatorLine

        color: "#21ffffff"

        width: 1
        visible: ( !isLast )
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        anchors.left: parent.right
    }
}
