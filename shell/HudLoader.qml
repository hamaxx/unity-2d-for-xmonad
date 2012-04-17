/*
 * This file is part of unity-2d
 *
 * Copyright 2012 Canonical Ltd.
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
import "common/utils.js" as Utils

Loader {
    id: hudLoader
    property bool animating: item.animating
    property bool active: item.active
    onActiveChanged: item.active = active

    source: "hud/Hud.qml"
    anchors.top: parent != undefined ? parent.top : undefined
    x: {
        if (parent != undefined && parent.launcherAlwaysVisible) {
            return Utils.isLeftToRight() ? launcherLoader.width : (parent != undefined ? parent.width - width - launcherLoader.width : 0)
        } else {
            return Utils.isLeftToRight() ? 0 : (parent != undefined ? parent.width - width : 0)
        }
    }
    onLoaded: item.focus = true
    visible: item.active
    focus: item.active
    width: Math.min(shell.width, (launcher2dConfiguration.hideMode == 0) ? 1061 - launcherLoader.width : 1061)
}
