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

/*
 * Modified by:
 * - Jure Ham <jure@hamsworld.net>
 */

import QtQuick 1.0
import "utils.js" as Utils

/*
Each SpreadWindow represents a real window on the desktop (we are
passed a WindowInfo object with all the information about it).

Its state ("" or "spread") decides which mode the window should
follow to position itself on screen ("screen" or "spread" mode
respectively).

In screen mode we use the real window's position and size to exactly mimic it.

In spread mode, we are assigned a cell in the spread, and we resize
and reposition ourselves to be fully constrained and centered inside
that specific cell.
The shot should occupy as much space as possible inside the cell,
but never be bigger than its original window's size, and always
maintain the same aspect ratio as the original window.
*/

Item {
    id: window

    property variant windowInfo
    property bool animating

    /* Maintain the selection status of this item to adjust visual appearence,
       but never change it from inside the component. Since all selection logic
       need to be managed outside of the component due to interaction with keyboard,
       we just forward mouse signals. */
    property bool isSelected

    signal clicked
    signal entered
    signal exited

    property bool enableBehaviors: false

    Rectangle {
        id: glow
		width: parent.width
		height: parent.height - 30
		smooth: true

		color: Qt.rgba(1, 1, 1, 0.3)
		radius: 3
		border.color: "#ffffff"
		border.width: 1
        visible: window.isSelected
    }

    /* Screenshot of the window, minus the decorations. The actual image is
       obtained via the WindowImageProvider which serves the "image://window/*" source URIs.
       Please note that the screenshot is taken at the moment the source property is
       actually assigned, during component initialization.
       If taking the screenshot fails (for example for minimized windows), then this
       is hidden and the icon box (see "icon_box" below) is shown. */
    Image {
        id: shot

        anchors.fill: parent
		anchors.margins: 5
		anchors.bottomMargin: 35
        fillMode: Image.Stretch

        /* HACK: QML uses an internal cache for Image objects that seems to use as
           key the source property of the image.
           This is great for normal images but in this case we really want the
           screenshot to reload everytime.
           Since I could not find any way to disable this cache, I am using this
           hack which essentially appends the current time to the source URL of the
           Image, tricking the cache into doing a request to the image provider.
        */
        source: "image://window/" + windowInfo.decoratedXid + "|"
                                  + windowInfo.contentXid + "@"
                                  + screen.currentTime()

        /* Disabled during animations for performance reasons */
        smooth: !animating

        visible: (status != Image.Error)
    }

    /* This replaces the shot whenever retrieving its image fails.
       It is essentially a white rectangle of the same size as the shot,
       with a border and the window icon floating in the center.
    */
    Rectangle {
        id: iconBox

        anchors.fill: parent
		anchors.margins: 5
		anchors.bottomMargin: 35

        border.width: 1
        border.color: "black"
        color: "#333"
		radius: 10


        visible: (shot.status == Image.Error)

        Image {
            source: "image://icons/" + windowInfo.icon
            asynchronous: true

            anchors.centerIn: parent
            fillMode: Image.PreserveAspectFit

            /* Please note that sourceSize is necessary, otherwise the
               IconImageProvider will crash when loading the icon */
            height: 64
            width: 64
            sourceSize { width: width; height: height }
        }
    }

	Text {
		id: label

		anchors.centerIn: parent
		anchors.verticalCenterOffset: parent.height / 2 - 17
		width: shot.width
		visible: true

		font.bold: true
		font.pointSize: 11

		text: windowInfo.title
		elide: Text.ElideRight
		horizontalAlignment: Text.AlignHCenter

		property real originalFontSize
		Component.onCompleted: {
			originalFontSize = font.pointSize
		}

		color: "white"
	}

    MouseArea {
        id: mouseArea

        width: shot.paintedWidth
        height: shot.paintedHeight
        anchors.centerIn: parent
        hoverEnabled: true

        onClicked: window.clicked()
        onEntered: window.entered()
        onExited: window.exited()
    }
}
