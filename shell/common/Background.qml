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

import QtQuick 1.1
import Unity2d 1.0
import Effects 1.0

Item {
    id: background
    property bool active: false
    property bool fullscreen: false
    property int horizontalOffset: 0

    anchors.fill: parent

    /* Avoid redraw at rendering */
    effect: CacheEffect {}

    Item {
        anchors.fill: parent
        anchors.bottomMargin: 39
        anchors.rightMargin: 37
        clip: true

        Image {
            id: blurredBackground

            effect: Blur {blurRadius: 12}

            /* 'source' needs to be set when this becomes visible, that is when active
               becomes true, so that a screenshot of the desktop is taken at that point.
               See http://doc.qt.nokia.com/4.7-snapshot/qml-image.html#cache-prop
            */

            /* Use an image of the root window which essentially is a
               capture of the entire screen */
            source: active ? "image://window/root" : ""
            cache: false

            fillMode: Image.PreserveAspectCrop
            x: -horizontalOffset
            y: -declarativeView.globalPosition.y
        }

        Image {
            anchors.fill: parent
            fillMode: Image.PreserveAspectCrop
            source: "artwork/background_sheen.png"
        }
    }

    BorderImage {
        anchors.fill: parent
        visible: !fullscreen
        source: screen.isCompositingManagerRunning ? "artwork/desktop_dash_background.sci" : "artwork/desktop_dash_background_no_transparency.sci"
        mirror: isRightToLeft()
    }
}
