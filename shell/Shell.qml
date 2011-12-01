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

    Launcher {
        id: launcher

        anchors.top: parent.top
        anchors.bottom: parent.bottom
        width: 66
        x: launcher.shown ? 0 : -width
        showMenus: !dashLoader.item.active

        Behavior on x { NumberAnimation { duration: 125 } }
        KeyNavigation.right: dashLoader
    }

    Loader {
        id: dashLoader
        source: "dash/Dash.qml"
        anchors.top: parent.top
        x: launcher.width
        KeyNavigation.left: launcher
        onLoaded: item.focus = true
        opacity: item.active ? 1.0 : 0.0
    }

    Connections {
        target: declarativeView
        onDashActiveChanged: if (declarativeView.dashActive) dashLoader.focus = true
        onLauncherFocusRequested: {
            launcher.focus = true
            launcher.focusBFB()
        }
        onFocusChanged: {
            /* FIXME: The launcher is forceVisible while it has activeFocus. However even though
               the documentation says that setting focus=false will make an item lose activeFocus
               if it has it, this doesn't happen with FocusScopes (and Launcher is a FocusScope).
               Therefore I'm working around this by giving focus to the shell, which is safe since
               the shell doesn't react to activeFocus at all.
               See: https://bugreports.qt.nokia.com/browse/QTBUG-19688 */
            if (!declarativeView.focus && launcher.activeFocus) shell.focus = true
        }
    }

    Component.onCompleted: declarativeView.show()
}
