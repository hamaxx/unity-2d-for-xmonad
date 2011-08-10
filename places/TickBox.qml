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
    id: tickBox

    property string text
    property bool checked: false
    property bool canUncheck: true

    Accessible.name: text
    Accessible.role: Accessible.CheckBox

    width: childrenRect.width
    height: childrenRect.height

    effect: DropShadow {
         blurRadius: 8
         color: "white"
         offset.x: 0
         offset.y: 0
         enabled: ( tickBox.state == "selected" )
    }

    Rectangle {
        id: container
        width: parent.width //should be 144
        height: parent.height //should be 29
        border.color: "white"
        border.width: 1
        color: ( checked ) ? "white" : "#00000000"
        opacity: if (checked) return 0.8
                 else if (parent.state == "selected") return 1
                 else return 0.5
        radius: 5
    }

    TextCustom {
        id: label
        anchors.fill: container
        width: parent.width
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        font.pixelSize: 15
        color: ( checked ) ? "black" : "white"
        text: tickBox.text
        elide: Text.ElideRight
        opacity: ( !canUncheck ) ? 0 : 1
    }
}
