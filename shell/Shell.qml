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

Item {
    id: shell
    height: childrenRect.height

    Binding {
        target: shell
        property: "width"
        when: launcher.progress === 1.0
        /* Clamp to a minimum width of 1px to make sure that the -1px margin is
           correctly considered (it's not reflected in the childrenRect) */
        value: Math.max(shell.childrenRect.width + Math.min(launcher.item.x, 0), 1)
    }

    Accessible.name: "shell"

    Loader {
        id: launcher
        source: "launcher/Launcher.qml"
        anchors.top: parent.top
        KeyNavigation.right: dash
        onLoaded: item.focus = true
        onActiveFocusChanged: {
            if (activeFocus) item.visibilityController.beginForceVisible("launcher")
            else item.visibilityController.endForceVisible("launcher")
        }
    }

    Loader {
        id: dash
        source: "dash/Dash.qml"
        anchors.top: parent.top
        anchors.left: launcher.right
        KeyNavigation.left: launcher
        onLoaded: dash.item.focus = true
        opacity: declarativeView.dashActive ? 1.0 : 0.0
    }

    Connections {
        target: declarativeView
        onDashActiveChanged: if (declarativeView.dashActive) dash.focus = true
        onLauncherFocusRequested: launcher.focus = true
        onFocusChanged: {
            /* FIXME: The launcher is forceVisible while it has activeFocus. However even though
               the documentation says that setting focus=false will make an item lose activeFocus
               if it has it, this doesn't happen with FocusScopes (and Loader is a FocusScope).
               Therefore I'm working around this by giving focus to the shell, which is safe since
               the shell doesn't react to activeFocus at all.
               See: https://bugreports.qt.nokia.com/browse/QTBUG-19688 */
            if (!declarativeView.focus && launcher.activeFocus) shell.focus = true
        }
    }

    Component.onCompleted: declarativeView.show()
}
