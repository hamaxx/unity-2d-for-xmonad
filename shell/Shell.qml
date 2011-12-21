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
import "launcher"

Item {
    id: shell
    height: screen.availableGeometry.height
    width: shell.childrenRect.width

    Accessible.name: "shell"

    LauncherLoader {
        id: launcherLoader
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        width: launcher2dConfiguration.thickness
        x: visibilityController.shown ? 0 : -width

        KeyNavigation.right: dashLoader

        Binding {
            target: launcherLoader.item
            property: "showMenus"
            value: !dashLoader.item.active
        }

        Behavior on x { NumberAnimation { duration: 125 } }

        onLoaded: if (declarativeView.dashActive) {
            launcherLoader.visibilityController.forceVisibleBegin("dash")
        }

        Connections {
            target: declarativeView
            onDashActiveChanged: {
                if (declarativeView.dashActive) launcherLoader.visibilityController.beginForceVisible("dash")
                else launcherLoader.visibilityController.endForceVisible("dash")
            }
        }

        SpreadMonitor {
            id: spread
            enabled: true
            onShownChanged: if (shown) {
                                /* The the spread grabs input and Qt can't properly
                                   detect we've lost input, so explicitly hide the menus */
                                launcherLoader.item.hideMenu()
                                launcherLoader.visibilityController.beginForceVisible("spread")
                            }
                            else launcherLoader.visibilityController.endForceVisible("spread")
        }
    }


    Loader {
        id: dashLoader
        source: "dash/Dash.qml"
        anchors.top: parent.top
        x: launcherLoader.width
        KeyNavigation.left: launcherLoader
        onLoaded: item.focus = true
        opacity: item.active ? 1.0 : 0.0
        focus: item.active

        Binding {
            target: dashLoader.item
            property: "fullscreenWidth"
            value: screen.availableGeometry.width - launcherLoader.width
        }
    }

    Connections {
        target: declarativeView
        onLauncherFocusRequested: {
            launcherLoader.focus = true
            launcherLoader.item.focusBFB()
        }
        onFocusChanged: {
            /* FIXME: The launcher is forceVisible while it has activeFocus. However even though
               the documentation says that setting focus=false will make an item lose activeFocus
               if it has it, this doesn't happen with FocusScopes (and Launcher is a FocusScope).
               Therefore I'm working around this by giving focus to the shell, which is safe since
               the shell doesn't react to activeFocus at all.
               See: https://bugreports.qt.nokia.com/browse/QTBUG-19688 */
            if (!declarativeView.focus && launcherLoader.activeFocus) shell.focus = true
        }
    }

    Component.onCompleted: declarativeView.show()

    Keys.onPressed: {
        if (event.key == Qt.Key_Escape) {
            declarativeView.forceDeactivateWindow()
        }
    }
}
