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
import Unity2d 1.0
import "launcher"
import "common/utils.js" as Utils

Item {
    id: shell
    /* Space reserved by strutManager is taken off screen.availableGeometry but
       we want the shell to take all the available space, including the one we
       reserved ourselves via strutManager. */
    height: declarativeView.screen.availableGeometry.height
    // We need the min because X is async thus it can happen that even if strutManager.enabled is true
    // declarativeView.screen.availableGeometry.width still has not been updated and thus the sum might be bigger than declarativeView.screen.geometry.width.
    // The real nice and proper solution would be strutManager having a hasTheChangeBeenApplied property
    width: Math.min(declarativeView.screen.geometry.width, declarativeView.screen.availableGeometry.width + (strutManager.enabled ? strutManager.width : 0))

    Accessible.name: "shell"

    property alias hudActive: hudLoader.active

    GestureHandler {
        id: gestureHandler
    }

    LauncherLoader {
        id: launcherLoader
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        width: 65

        /* Launcher visibility handling and 4 fingers dragging reveal */
        property int hiddenX: Utils.isLeftToRight() ? -width : shell.width
        property int shownX: Utils.isLeftToRight() ? 0 : shell.width - width
        x: {
            var value
            var delta = Utils.isLeftToRight() ? gestureHandler.dragDelta : -gestureHandler.dragDelta

            if (visibilityController.shown) {
                value = shownX
            } else {
                /* FIXME: it would be better to have gestureHandler disabled
                   for dragging when hideMode is set to 0 */
                if (launcher2dConfiguration.hideMode != 0 && gestureHandler.isDragging) {
                    value = hiddenX + delta
                } else {
                    value = hiddenX
                }
            }

            if (hiddenX <= shownX) {
                return Utils.clamp(value, hiddenX, shownX)
            } else {
                return Utils.clamp(value, shownX, hiddenX)
            }
        }

        KeyNavigation.right: dashLoader

        Binding {
            target: launcherLoader.item
            property: "showMenus"
            value: !dashLoader.item.active && !hudLoader.item.active
        }

        Behavior on x { NumberAnimation { id: launcherLoaderXAnimation; duration: 125 } }

        Connections {
            target: declarativeView
            onDashActiveChanged: {
                if (declarativeView.dashActive) {
                    if (hudLoader.item.active) hudLoader.item.active = false
                    launcherLoader.visibilityController.beginForceVisible("dash")
                } else {
                    launcherLoader.visibilityController.endForceVisible("dash")
                    if (dashLoader.status == Loader.Ready) dashLoader.item.deactivateAllLenses()
                }
            }
            onGlobalPositionChanged: {
                var x = declarativeView.globalPosition.x + (Utils.isLeftToRight() ? 0 : shell.width)
                launcherLoader.item.barrierP1 = Qt.point(x, 0)
                launcherLoader.item.barrierP2 = Qt.point(x, declarativeView.screen.geometry.height)
                launcherLoader.item.barrierTriggerZoneP1 = Qt.point(x, declarativeView.globalPosition.y)
                launcherLoader.item.barrierTriggerZoneP2 = Qt.point(x, declarativeView.globalPosition.y + launcherLoader.height)
            }
        }

        SpreadMonitor {
            id: spread
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
        x: Utils.isLeftToRight() ? launcherLoader.width : shell.width - width - launcherLoader.width
        onLoaded: item.focus = true
        opacity: item.active ? 1.0 : 0.0
        focus: item.active

        Binding {
            target: dashLoader.item
            property: "fullscreenWidth"
            value: shell.width - launcherLoader.width
        }
    }

    Loader {
        id: hudLoader
        property bool animating: item.animating
        property bool active: item.active
        onActiveChanged: item.active = active

        source: "hud/Hud.qml"
        anchors.top: parent.top
        x: Utils.isLeftToRight() ? 0 : shell.width - width
        onLoaded: item.focus = true
        visible: item.active
        focus: item.active
        width: Math.min(shell.width, 1061)
    }

    Connections {
        target: hudLoader.item
        onActiveChanged: {
            if (hudLoader.item.active) {
                if (dashLoader.item.active) dashLoader.item.active = false
                launcherLoader.visibilityController.beginForceHidden("hud")
            } else {
                launcherLoader.visibilityController.endForceHidden("hud")
            }
        }
    }

    Connections {
        target: declarativeView
        onLauncherFocusRequested: {
            launcherLoader.focus = true
            launcherLoader.item.focusBFB()
        }
        onFocusChanged: {
            if (!declarativeView.focus && hudLoader.item.active) hudLoader.item.active = false

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
        if (event.key == Qt.Key_Escape || (event.key == Qt.Key_F4 && event.modifiers == Qt.AltModifier )) {
            declarativeView.forceDeactivateWindow()
        }
    }

    InputShapeManager {
        target: declarativeView

        InputShapeRectangle {
            id: launcherInputShape
            enabled: launcherLoader.status == Loader.Ready
        }

        InputShapeRectangle {
            rectangle: if (desktop.isCompositingManagerRunning) {
                Qt.rect(dashLoader.x, dashLoader.y, dashLoader.width, dashLoader.height)
            } else {
                Qt.rect(dashLoader.x, dashLoader.y, dashLoader.width - 7, dashLoader.height - 9)
            }
            enabled: dashLoader.status == Loader.Ready && dashLoader.item.active
            mirrorHorizontally: Utils.isRightToLeft()

            InputShapeMask {
                id: shape1
                source: "shell/common/artwork/desktop_dash_background_no_transparency.png"
                color: "red"
                position: Qt.point(dashLoader.width - 50, dashLoader.height - 49)
                enabled: declarativeView.dashMode == ShellDeclarativeView.DesktopMode
            }
        }

        InputShapeRectangle {
            id: hudInputShape
            enabled: hudLoader.status == Loader.Ready && hudLoader.item.active

            InputShapeMask {
                source: "shell/common/artwork/desktop_dash_background_no_transparency.png"
                color: "red"
                position: Qt.point(hudLoader.width - 50, hudLoader.height - 49)
            }
        }
    }

    Binding {
        target: launcherInputShape
        property: "rectangle"
        value: Qt.rect(launcherLoader.x,
                       launcherLoader.y,
                       launcherLoader.width,
                       launcherLoader.height)
        when: !launcherLoaderXAnimation.running
    }

    Binding {
        target: hudInputShape
        property: "rectangle"
        value: {
            if (desktop.isCompositingManagerRunning) {
                return Qt.rect(hudLoader.x, hudLoader.y, hudLoader.width, hudLoader.height)
            } else {
                return Qt.rect(hudLoader.x, hudLoader.y, hudLoader.width - 7, hudLoader.height - 9)
            }
        }
        when: !hudLoader.animating
    }

    StrutManager {
        id: strutManager
        edge: Unity2dPanel.LeftEdge
        widget: declarativeView
        height: launcherLoader.height
        width: launcherLoader.width
        enabled: Utils.clamp(launcher2dConfiguration.hideMode, 0, 2) == 0
    }
}
