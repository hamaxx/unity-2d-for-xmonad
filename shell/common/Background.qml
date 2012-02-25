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
import "utils.js" as Utils

Item {
    id: background
    property bool active: false
    property bool fullscreen: false
    property int bottomBorderThickness
    property int rightBorderThickness

    /* Avoid redraw at rendering */
    effect: CacheEffect {}

    Item {
        anchors.fill: parent
        anchors.bottomMargin: bottomBorderThickness
        anchors.rightMargin: rightBorderThickness
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

            /* Place the screenshot of the desktop background on top of the desktop background,
               no matter where the DeclarativeView or the parent object are placed.
            */
            property variant origin: parent.mapFromItem(null, -declarativeView.globalPosition.x, -declarativeView.globalPosition.y)
            x: origin.x
            y: origin.y
        }

        Image {
            anchors.fill: parent
            fillMode: Image.PreserveAspectCrop
            source: "artwork/background_sheen.png"
        }
    }

    BorderImage {
        id: border

        /* Define properties of border here */
        property int bottomThickness: 39
        property int rightThickness: 37

        anchors.fill: parent
        source: desktop.isCompositingManagerRunning ? "artwork/desktop_dash_background.sci" : "artwork/desktop_dash_background_no_transparency.sci"
        mirror: Utils.isRightToLeft()
    }

    states: [
        State {
            name: "normal"
            when: !fullscreen
            PropertyChanges {
                target: background
                bottomBorderThickness: border.bottomThickness
                rightBorderThickness: border.rightThickness
            }
            PropertyChanges {
                target: border
                visible: true
            }
        },
        State {
            name: "fullscreen"
            when: fullscreen
            PropertyChanges {
                target: background
                bottomBorderThickness: 0
                rightBorderThickness: 0
            }
            PropertyChanges {
                target: border
                visible: false
            }
        }
    ]
}
