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
            value: !dashLoader.item.active
        }

        Behavior on x { NumberAnimation { id: launcherLoaderXAnimation; duration: 125 } }

        Connections {
            target: declarativeView
            onDashActiveChanged: {
                if (declarativeView.dashActive) launcherLoader.visibilityController.beginForceVisible("dash")
                else {
                    launcherLoader.visibilityController.endForceVisible("dash")
                    if (dashLoader.status == Loader.Ready) dashLoader.item.deactivateAllLenses()
                }
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
                source: "shell/dash/artwork/desktop_dash_background_no_transparency.png"
                color: "red"
                position: Qt.point(dashLoader.width - 50, dashLoader.height - 49)
                enabled: declarativeView.dashMode == ShellDeclarativeView.DesktopMode
            }
        }
    }

    Binding {
        target: launcherInputShape
        property: "rectangle"
        value: {
            // FIXME: this results in a 1px wide white rectangle on the launcher edge, we should switch
            //        to cpp-based edge detection, and later XFixes barriers to get rid of that completely
            var somewhatShown = Utils.isLeftToRight() ? -launcherLoader.x < launcherLoader.width : launcherLoader.x < shell.width
            if (somewhatShown) {
                return Qt.rect(launcherLoader.x,
                                launcherLoader.y,
                                launcherLoader.width,
                                launcherLoader.height)
            } else {
                // The outerEdgeMouseArea is one pixel bigger on each side so use it
                // when the launcher is hidden to have that extra pixel in the border
                return Qt.rect(launcherLoader.x + launcherLoader.outerEdgeMouseArea.x,
                                launcherLoader.y,
                                launcherLoader.outerEdgeMouseArea.width,
                                launcherLoader.height)
            }
        }
        when: !launcherLoaderXAnimation.running
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
