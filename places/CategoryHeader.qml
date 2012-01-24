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
    id: categoryHeader

    property string icon
    property alias label: title.text
    property bool folded: true
    property int availableCount
    /* HACK: DropShadow causes visual artifacts while being moved. The previously painted
       DropShadow can remain in areas where nothing draws over it. Work around this by
       detecting the flicking movement and disable DropShadow when it happens.
       DropShadow not officially supported until Qt4.8, when hopefully this will be fixed.
    */
    property bool moving: false

    Accessible.name: "%1 %2 %3".arg(title.text).arg(label.text).arg(folded ? u2d.tr("not expanded") : u2d.tr("expanded"))

    effect: DropShadow {
                blurRadius: 6
                offset.x: 0
                offset.y: 0
                color: "white"
                enabled: categoryHeader.state == "pressed"
            }

    Image {
        id: iconImage

        source: icon

        width: 22
        height: 22
        anchors.bottom: underline.top
        anchors.bottomMargin: 5
        anchors.left: parent.left
        anchors.leftMargin: 8
        fillMode: Image.PreserveAspectFit
        sourceSize.width: width
        sourceSize.height: height
    }

    TextCustom {
        id: title

        fontSize: "large"
        anchors.baseline: underline.top
        anchors.baselineOffset: -10
        anchors.left: iconImage.right
        anchors.leftMargin: 8
    }

    Item {
        id: moreResults

        visible: availableCount > 0
        anchors.left: title.right
        anchors.leftMargin: 11
        anchors.baseline: title.baseline

        opacity: ( categoryHeader.state == "selected" || categoryHeader.state == "pressed"
                  || categoryHeader.state == "hovered" ) ? 1.0 : 0.5
        Behavior on opacity {NumberAnimation { duration: 100 }}

        effect: DropShadow {
                    blurRadius: 4
                    offset.x: 0
                    offset.y: 0
                    color: "white"
                    enabled: ( moreResults.opacity == 1.0 && !moving )
                }

        TextCustom {
            id: label

            fontSize: "small"
            text: if(categoryHeader.folded) {
                      return u2d.tr("See %1 more result", "See %1 more results", availableCount).arg(availableCount)
                  } else {
                      return u2d.tr("See fewer results")
                  }

            anchors.left: parent.left
            anchors.baseline: parent.baseline
        }

        FoldingArrow {
            id: arrow

            folded: categoryHeader.folded
            anchors.left: label.right
            anchors.leftMargin: 10
            anchors.verticalCenter: label.verticalCenter
        }
    }

    Rectangle {
        id: underline

        color: "#21ffffff"

        height: 1
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        anchors.right: parent.right
    }
}
