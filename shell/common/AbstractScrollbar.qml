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

/* Scrollbar composed of:
   - a draggable slider

   Usage:

   Flickable {
       id: flickable
   }

   Scrollbar {
       targetFlickable: flickable
   }
*/
Item {
    id: scrollbar

    property variant targetFlickable
    property alias sliderSource: sliderImage.source
    property alias sliderAnchors: sliderImage.anchors
    property alias sliderSmooth: sliderImage.smooth

    MouseArea {
        id: scrollMouseArea

        anchors.fill: parent
        acceptedButtons: Qt.LeftButton | Qt.MiddleButton | Qt.RightButton

        onPressed: {
            /* Scroll one page without overshooting */
            var scrollAmount = mouseY > slider.y ? targetFlickable.height : -targetFlickable.height
            var destination = targetFlickable.contentY + scrollAmount
            var clampedDestination = Math.max(0, Math.min(targetFlickable.contentHeight - targetFlickable.height,
                                                          destination))
            scrollAnimation.to = clampedDestination
            scrollAnimation.restart()
        }

        NumberAnimation {
            id: scrollAnimation

            duration: 200
            easing.type: Easing.InOutQuad
            target: targetFlickable
            property: "contentY"
        }
    }

    Item {
        id: slider

        property int minimalHeight: 40

        anchors.left: parent.left
        anchors.right: parent.right

        Binding {
            target: slider
            property: "y"
            value: {
                var clampedYPosition = Math.max(0, Math.min(1-targetFlickable.visibleArea.heightRatio,
                                                            targetFlickable.visibleArea.yPosition))
                return clampedYPosition * scrollbar.height
            }
            when: !dragMouseArea.drag.active
        }

        height: Math.min(scrollbar.height, Math.max(minimalHeight, targetFlickable.visibleArea.heightRatio * scrollbar.height))

        Behavior on height {NumberAnimation {duration: 200; easing.type: Easing.InOutQuad}}

        BorderImage {
            id: sliderImage

            anchors.fill: parent
        }

        MouseArea {
            id: dragMouseArea

            anchors.fill: parent
            hoverEnabled: true
            acceptedButtons: Qt.LeftButton | Qt.MiddleButton | Qt.RightButton
            drag.target: slider
            drag.axis: Drag.YAxis
            drag.minimumY: 0
            drag.maximumY: scrollbar.height - slider.height

            onPositionChanged: {
                if (drag.active) {
                    targetFlickable.contentY = slider.y * targetFlickable.contentHeight / scrollbar.height
                }
            }
        }
    }
}
