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

import QtQuick 1.0
import Unity2d 1.0
import "../common"
import "../common/utils.js" as Utils

Loader {
    property bool launcherOnlyInOneScreen: false // TODO Read this from dconf
    property bool loadLauncher: !launcherOnlyInOneScreen || declarativeView.screen.screen == 0
    id: launcherLoader
    source: loadLauncher ? "Launcher.qml" : ""
    property variant visibilityController: visibilityController
    onLoaded: item.focus = true
    property bool launcherAlwaysVisible: Utils.clamp(launcher2dConfiguration.hideMode, 0, 2) == 0

    VisibilityController {
        id: visibilityController
        behavior: launcherBehavior.status == Loader.Ready ? launcherBehavior.item : null
    }

    Loader {
        id: launcherBehavior

        property variant modesMap: { 0: '../common/visibilityBehaviors/AlwaysVisibleBehavior.qml',
                                     1: '../common/visibilityBehaviors/AutoHideBehavior.qml',
                                     2: '../common/visibilityBehaviors/IntelliHideBehavior.qml' }

        source: modesMap[Utils.clamp(launcher2dConfiguration.hideMode, 0, 2)]
    }

    Binding {
        target: launcherBehavior
        property: "item.target"
        value: launcherLoader.item
        when: launcherBehavior.status == Loader.Ready
    }

    Binding {
        target: declarativeView
        property: "monitoredArea"
        value: loadLauncher ? Qt.rect(launcherLoader.x, launcherLoader.item.y, launcherLoader.item.width, launcherLoader.item.height) : Qt.rect(0, 0, 0, 0)
        when: launcherBehavior.status == Loader.Ready && !launcherLoaderXAnimation.running
    }

    Binding {
        target: launcherBehavior.item
        property: "forcedVisible"
        value: visibilityController.forceVisible
    }

    Binding {
        target: launcherBehavior.item
        property: "forcedVisibleChangeId"
        value: visibilityController.forceVisibleChangeId
    }

    Binding {
        target: launcherBehavior.item
        property: "forcedHidden"
        value: visibilityController.forceHidden
    }

    Connections {
        target: shellManager
        onSuperKeyHeldChanged: {
            if (superKeyHeld) visibilityController.beginForceVisible()
            else visibilityController.endForceVisible()
        }
    }

    Binding {
        target: launcherLoader.item
        property: "shown"
        value: visibilityController.shown
    }
}
