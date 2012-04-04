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
import Effects 1.0
import "../common"

AbstractButton {
    id: multiRangeButton

    property string text
    property bool checked: false
    property bool isLast: false

    Accessible.name: text

    TextCustom {
        id: label
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        anchors.leftMargin: 1
        width: parent.width-1
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        fontSize: "small"
        color: "white"
        text: multiRangeButton.text
        elide: Text.ElideRight
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
