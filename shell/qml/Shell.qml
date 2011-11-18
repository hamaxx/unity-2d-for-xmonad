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
    }

    Loader {
        id: dash
        source: "dash/Dash.qml"
        anchors.top: parent.top
        anchors.left: launcher.right
    }

    Component.onCompleted: declarativeView.show()
}
