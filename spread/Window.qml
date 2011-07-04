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

    /* Screenshot of the window, minus the decorations. The actual image is
       obtained via the WindowImageProvider which serves the "image://window/*" source URIs.
       Please note that the screenshot is taken at the moment the source property is
       actually assigned, during component initialization.
       If taking the screenshot fails (for example for minimized windows), then this
       is hidden and the icon box (see "icon_box" below) is shown. */
    Image {
        id: shot

        anchors.fill: parent
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

        border.width: 1
        border.color: "black"

        visible: (shot.status == Image.Error)

        Image {
            source: "image://icons/" + windowInfo.icon
            asynchronous: true

            anchors.centerIn: parent
            fillMode: Image.PreserveAspectFit

            /* Please note that sourceSize is necessary, otherwise the
               IconImageProvider will crash when loading the icon */
            height: 48
            width: 48
            sourceSize { width: width; height: height }
        }
    }

    Item {
        id: overlay

        anchors.fill: parent

        /* A label with the window title centered over the shot.
           It will appear only for the currently selected window. See overlay.states */
        Rectangle {
            id: labelBox

            /* The width of the box around the text should be the same as
               the text itself, with 3 pixels of margin on all sides, but it should also
               never overflow the shot's borders.

               Normally one would just set anchors.margins, but this can't work
               here because first we need to let the Text calculate it's "natural" width
               ("paintedWidth" in QT terms) -- that is, the size it would have
               ìf free to expand horizontally unconstrained, to know if it's smaller than
               the labelBox or not.
               However if we bind the Text's width to the width of the labelBox, and the
               width of the labelBox to the Text's size, we get a binding loop error.

               The trick is to bind the Text's width to the labelBox's parent, and then
               the labelBox to the Text's size. Since the relation between labelBox and
               parent is taken care of by the positioner indirectly, there's no loop.

               Yeah, messy. Blame QML ;)
            */
            property int labelMargins: 6
            width: Math.min(parent.width, label.paintedWidth + labelMargins)
            height: label.height + labelMargins
            anchors.centerIn: parent

            /* This equals backgroundColor: "black" and opacity: 0.6
               but we don't want to set it that way since it would be
               inherited by the Text child, and we want it to be fully
               opaque instead */
            color: "#99000000"
            radius: 3
            visible: window.isSelected

            Text {
                id: label

                anchors.centerIn: parent
                width: overlay.width - parent.labelMargins

                text: windowInfo.title
                elide: Text.ElideRight
                horizontalAlignment: Text.AlignHCenter

                property real originalFontSize
                Component.onCompleted: {
                    originalFontSize = font.pointSize
                }

                color: "white"
            }
        }
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
