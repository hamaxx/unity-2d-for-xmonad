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

SearchRefineOption {
    id: searchRefineOption

    Item {
        id: header

        Accessible.name: searchRefineOption.title

        KeyNavigation.down: filters

        focus: true
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        height: childrenRect.height

        TextCustom {
            id: title

            anchors.top: parent.top
            anchors.left: parent.left

            text: searchRefineOption.title
            font.pixelSize: 16
            font.bold: true
            font.underline: ( parent.state == "selected" || parent.state == "hovered" )
        }
    }

    MultiRangeView {
        id: filters

        anchors.top: header.bottom
        anchors.topMargin: 15
        anchors.left: parent.left
        anchors.right: parent.right
        height: 30

        model: searchRefineOption.filterModel.options
    }
}
