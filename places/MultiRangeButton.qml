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

AbstractButton {
    id: multiRangeButton

    property string text
    property bool checked: false
    property bool canUncheck: true

    Accessible.name: text
    Accessible.role: Accessible.Slider

    width: childrenRect.width
    height: childrenRect.height

    effect: DropShadow {
         blurRadius: 8
         color: "white"
         offset.x: 0
         offset.y: 0
         enabled: ( multiRangeButton.state == "selected" )
    }

    Rectangle {
        id: container
        width: parent.width
        height: parent.height
        border.color: if ( parent.state == "selected") return "white"
                      else if ( checked ) return "#cdffffff" // 13% opaque
                      else return "#21ffffff" // 80% opaque
        border.width: ( checked ) ? 2 : -1
        color: ( checked ) ? "#21ffffff" : "transparent"
        //radius: 5
    }

    TextCustom {
        id: label
        anchors.fill: container
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        font.pixelSize: 12
        color: "white"
        text: multiRangeButton.text
        elide: Text.ElideRight
        opacity: ( !canUncheck ) ? 0 : 1
    }
}
