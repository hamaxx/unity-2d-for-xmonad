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

import QtQuick 1.1
import Unity2d 1.0
import "utils.js" as Utils
import Effects 1.0

Rectangle {
    id: switcher

    //color: "black"

    property string applicationFilter
    property int zoomedWorkspace: 0

    property variant allWindows: WindowsList { }
    property int lastActiveWindow: 0

    Repeater {
        id: workspaces

        model: 1
        delegate: Workspace {
            id: workspace
            width: switcher.width
            height: switcher.height
            focus: true
            state: "zoomed"

        }
    }
    Image {
        id: blurredBackground

        effect: Blur {blurRadius: 12}

        /* 'source' needs to be set when the dash becomes visible, that
           is when declarativeView.active becomes true, so that a
           screenshot of the windows behind the dash is taken at that
           point.
           'source' also needs to change so that the screenshot is
           re-taken as opposed to pulled from QML's image cache.
           This workarounds the fact that the image cache cannot be
           disabled. A new API to deal with this was introduced in Qt Quick 1.1.

           See http://doc.qt.nokia.com/4.7-snapshot/qml-image.html#cache-prop
        */
        property variant timeAtActivation

        /* Use an image of the root window which essentially is a
           capture of the entire screen */
        source: "image://window/root@" + blurredBackground.timeAtActivation

        fillMode: Image.PreserveAspectCrop
        x: -declarativeView.globalPosition.x
        y: -declarativeView.globalPosition.y
    }
    Image {
        anchors.fill: parent
        fillMode: Image.PreserveAspectCrop
        source: "artwork/background_sheen.png"
    }

    /* This connection receives all commands from the DBUS API */
    Connections {
        target: control

        onShowCurrentWorkspace: {
            applicationFilter = applicationDesktopFile
            zoomedWorkspace = 0
            show()
        }

        onShowAllWorkspaces: {
            applicationFilter = applicationDesktopFile
            zoomedWorkspace = 0
            show()
        }

        onHide: cancelAndExit()
        onFilterByApplication: applicationFilter = applicationDesktopFile
    }

    function show() {
        blurredBackground.timeAtActivation = screen.currentTime()
        lastActiveWindow = screen.activeWindow

        allWindows.load()

        spreadView.show()
        spreadView.forceActivateWindow()
        //switcher.forceActiveFocus()
    }

    Keys.onPressed: {
        switch (event.key) {
        case Qt.Key_Escape:
            cancelAndExit()
            event.accepted = true
            return
        case Qt.Key_S:
            if (event.modifiers & Qt.MetaModifier) {
                cancelAndExit()
                event.accepted = true
                return
            }
            break
        }
    }

    Connections {
        target: spreadView
        onOutsideClick: cancelAndExit()
    }

    function cancelAndExit() {
        spreadView.hide()
        //allWindows.unload() //caused segfault :(
        zoomedWorkspace = 0
    }

    function activateWindow(windowInfo) {
        windowInfo.activate()
        cancelAndExit()
    }
}
