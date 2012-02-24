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
    id: categoryHeader

    property string icon
    property alias label: title.text
    property bool folded: true
    property int availableCount

    Accessible.name: "%1 %2 %3".arg(title.text).arg(label.text).arg(folded ? u2d.tr("not expanded") : u2d.tr("expanded"))

    Image {
        id: iconImage

        source: icon

        width: 22
        height: 22
        anchors.bottom: underline.top
        anchors.bottomMargin: 5
        anchors.left: parent.left
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
        anchors.leftMargin: 14
        anchors.baseline: title.baseline

        opacity: ( categoryHeader.state == "selected" || categoryHeader.state == "pressed"
                  || categoryHeader.state == "hovered" ) ? 1.0 : 0.5
        Behavior on opacity {NumberAnimation { duration: 100 }}

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
            anchors.bottom: label.baseline
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
