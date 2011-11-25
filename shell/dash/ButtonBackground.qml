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

Item {
    id: background

    property string state

    opacity: ( state == "selected" || state == "pressed"
              || state == "hovered" ) ? 1.0 : 0.0
    Behavior on opacity {NumberAnimation {duration: 100}}

    Rectangle {

        /* FIXME: */
        anchors.fill: parent
        anchors.bottomMargin: 1
        anchors.rightMargin: 1

        color: background.state == "pressed" ? "#ffffffff" : "#00000000"
        border.color: "#cccccc"
        border.width: 1
        radius: 3

        Image {
            fillMode: Image.Tile
            anchors.fill: parent
            source: "artwork/button_background.png"
            smooth: false
        }
    }
}
