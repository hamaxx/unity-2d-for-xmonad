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

/* This component represents a single "lens" in the dash and an active
 * indicator icon positioned above it if necessary.
 */
AbstractButton {
    property alias icon: icon.source
    property bool active: false
    property int iconSpacing: 0
    property int iconWidth: 24

    id: lensButton

    effect: DropShadow {
         blurRadius: 8
         color: "white"
         offset.x: 0
         offset.y: 0
         enabled: ( lensButton.state == "selected" || active )
    }

    Rectangle {
        anchors.fill: parent
        anchors.topMargin: 7
        anchors.bottomMargin: 7
        anchors.leftMargin: Math.floor(iconSpacing/2)
        anchors.rightMargin: Math.ceil(iconSpacing/2)

        border.color: "white"
        border.width: 1
        color: "transparent"
        radius: 2
        visible: ( parent.state == "selected" )
    }

    Image {
        id: icon

        sourceSize.width: iconWidth
        sourceSize.height: iconWidth
        width: sourceSize.width
        height: sourceSize.height
        clip: true

        anchors.horizontalCenter: parent.horizontalCenter
        anchors.centerIn: parent

        opacity: ( parent.state == "mouseOver" || parent.state == "pressed" || active ) ? 1.0 : 0.57
    }

    /* Indicator arrow to show Lens active */
    Image {
        id: indicator
        source: "artwork/arrow.png"

        width: sourceSize.width
        height: sourceSize.height
        anchors.bottomMargin: (width-height)/2 /* Correct for rotation */
        anchors.bottom: parent.bottom
        anchors.horizontalCenter: parent.horizontalCenter

        rotation: -90
        visible: active
    }
}
