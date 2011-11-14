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
    width: childrenRect.width
    height: childrenRect.height

    Loader {
        id: launcher
        source: "launcher/Launcher.qml"
        anchors.top: parent.top
        anchors.left: parent.left
    }

    Loader {
        id: dash
        focus: true
        source: "dash/dash.qml"
        anchors.top: parent.top
    }

    Component.onCompleted: declarativeView.show()
}
