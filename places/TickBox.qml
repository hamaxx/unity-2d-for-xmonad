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
    id: tickBox

    property string text
    property bool checked: false
    property bool canUncheck: true

    width: childrenRect.width
    height: childrenRect.height

    TextCustom {
        id: label

        anchors.left: box.right
        anchors.leftMargin: 8
        anchors.right: tickBox.right
        anchors.top: parent.top
        font.pixelSize: 16
        text: tickBox.text
        elide: Text.ElideRight
    }

    Image {
        id: box

        opacity: !canUncheck && checked ? 0 : 1
        anchors.top: parent.top
        anchors.left: parent.left
        source: "artwork/tick_box.png"
        width: sourceSize.width
        height: sourceSize.height
    }

    Image {
        id: tick

        anchors.top: box.top
        anchors.topMargin: 2
        anchors.left: box.left
        anchors.leftMargin: 3
        opacity: checked ? 1.0 : parent.state == "selected" ? 0.4 : 0.0
        source: "artwork/tick.png"
        width: sourceSize.width
        height: sourceSize.height
    }
}
