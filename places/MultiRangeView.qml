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

FocusScope {
    id: multiRangeView

    property alias model: list.model

    Rectangle {
        id: container

        anchors.fill: parent

        border.color: "#21ffffff" // 80% opaque white
        border.width: 1
        color: "transparent"
        radius: 5
    }

    ListView {
        id: list

        anchors.fill: container
        orientation: ListView.Horizontal

        delegate: MultiRangeButton {
            width: 45
            height: 30
            text: item.name
            checked: item.active
            onClicked: item.active = !item.active
        }
    }
}

