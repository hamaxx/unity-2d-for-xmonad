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
import Unity2d 1.0
import "utils.js" as Utils

Rectangle {
    id: switcher

    color: "black"

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
        lastActiveWindow = screen.activeWindow

        allWindows.load()

        spreadView.show()
        spreadView.forceActivateWindow()
        switcher.forceActiveFocus()
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
