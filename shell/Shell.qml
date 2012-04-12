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

    property variant declarativeView
    property variant dashLoader
    property variant hudLoader
    property bool launcherAlwaysVisible: launcherLoader.loadLauncher && !launcherLoader.launcherInHideMode

    Binding {
        target: declarativeView
        property: "accessibleName"
        value: {
            if (shellManager.hudActive) {
                return "Hud";
            }
            if (shellManager.dashActive) {
                return "Dash";
            }
            return "Launcher";
        }
    }

    /* Space reserved by strutManager is taken off screen.availableGeometry but
       we want the shell to take all the available space, including the one we
       reserved ourselves via strutManager. */
    height: declarativeView.screen.availableGeometry.height
    // We need the min because X is async thus it can happen that even if strutManager.enabled is true
    // declarativeView.screen.availableGeometry.width still has not been updated and thus the sum might be bigger than declarativeView.screen.geometry.width.
    // The real nice and proper solution would be strutManager having a hasTheChangeBeenApplied property
    width: Math.min(declarativeView.screen.geometry.width, declarativeView.screen.availableGeometry.width + (strutManager.enabled ? strutManager.width : 0))

    Accessible.name: "shell"

    WallpaperColor {
    }

    GestureHandler {
        id: gestureHandler
    }

    onDashLoaderChanged: {
        if (shellManager.dashActive) {
            if (dashLoader == undefined)
            {
                launcherLoader.visibilityController.endForceVisible("dash")
            } else {
                launcherLoader.visibilityController.beginForceVisible("dash")
            }
        }
    }

    onHudLoaderChanged: {
        if (shellManager.hudActive && launcher2dConfiguration.hideMode != 0) {
            if (hudLoader == undefined)
            {
                launcherLoader.visibilityController.endForceHidden("hud")
            } else {
                launcherLoader.visibilityController.beginForceHidden("hud")
            }
        }
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

        Binding {
            target: launcherLoader.item
            property: "showMenus"
            value: (dashLoader == undefined || !dashLoader.item.active) && (hudLoader == undefined || !hudLoader.item.active)
        }

        Behavior on x { NumberAnimation { id: launcherLoaderXAnimation; duration: 125 } }

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

    Connections {
        target: shellManager

        onDashActiveChanged: {
            if (shellManager.dashActive) {
                if (hudLoader != undefined && hudLoader.item.active) {
                    hudLoader.item.active = false
                }
                if (dashLoader != undefined) {
                    launcherLoader.visibilityController.beginForceVisible("dash")
                }
            } else {
                if (dashLoader != undefined) {
                    launcherLoader.visibilityController.endForceVisible("dash")
                    if (dashLoader.status == Loader.Ready) dashLoader.item.deactivateAllLenses()
                }
            }
        }

        onHudActiveChanged: {
            if (shellManager.hudActive) {
                if (dashLoader != undefined && dashLoader.item.active) {
                    dashLoader.item.active = false
                }
                if (hudLoader != undefined && launcher2dConfiguration.hideMode != 0) {
                    launcherLoader.visibilityController.beginForceHidden("hud")
                }
            } else {
                if (hudLoader != undefined && launcher2dConfiguration.hideMode != 0) {
                    launcherLoader.visibilityController.endForceHidden("hud")
                }
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
            if (!declarativeView.focus && hudLoader!= undefined && hudLoader.item.active) hudLoader.item.active = false

            /* FIXME: The launcher is forceVisible while it has activeFocus. However even though
               the documentation says that setting focus=false will make an item lose activeFocus
               if it has it, this doesn't happen with FocusScopes (and Launcher is a FocusScope).
               Therefore I'm working around this by giving focus to the shell, which is safe since
               the shell doesn't react to activeFocus at all.
               See: https://bugreports.qt.nokia.com/browse/QTBUG-19688 */
            if (!declarativeView.focus && launcherLoader.activeFocus) shell.focus = true
        }
    }

    Component.onCompleted: {
        if (declarativeView.screen.screen == 0) {
            var loaderComponent = Qt.createComponent("DashLoader.qml");
            dashLoader = loaderComponent.createObject(shell, {});

            var loaderComponent = Qt.createComponent("HudLoader.qml");
            hudLoader = loaderComponent.createObject(shell, {});
        }
        declarativeView.show()
    }

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
            rectangle: {
                if (dashLoader != undefined) {
                    if (desktop.isCompositingManagerRunning) {
                        return Qt.rect(dashLoader.x, dashLoader.y, dashLoader.width, dashLoader.height)
                    } else {
                        return Qt.rect(dashLoader.x, dashLoader.y, dashLoader.width - 7, dashLoader.height - 9)
                    }
                } else {
                    return Qt.rect(0, 0, 0, 0)
                }
            }
            enabled: dashLoader != undefined && dashLoader.status == Loader.Ready && dashLoader.item.active
            mirrorHorizontally: Utils.isRightToLeft()

            InputShapeMask {
                id: shape1
                source: "shell/common/artwork/desktop_dash_background_no_transparency.png"
                color: "red"
                position: dashLoader != undefined ? Qt.point(dashLoader.width - 50, dashLoader.height - 49) : Qt.point(0, 0)
                enabled: shellManager.dashMode == ShellManager.DesktopMode
            }
        }

        InputShapeRectangle {
            id: hudInputShape
            enabled: hudLoader != undefined && hudLoader.status == Loader.Ready && hudLoader.item.active

            InputShapeMask {
                source: "shell/common/artwork/desktop_dash_background_no_transparency.png"
                color: "red"
                position: hudLoader != undefined ? Qt.point(hudLoader.width - 50, hudLoader.height - 49) : Qt.point(0, 0)
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
            if (hudLoader != undefined) {
                if (desktop.isCompositingManagerRunning) {
                    return Qt.rect(hudLoader.x, hudLoader.y, hudLoader.width, hudLoader.height)
                } else {
                    return Qt.rect(hudLoader.x, hudLoader.y, hudLoader.width - 7, hudLoader.height - 9)
                }
            } else {
                return Qt.rect(0, 0, 0, 0)
            }
        }
        when: hudLoader != undefined && !hudLoader.animating
    }

    StrutManager {
        id: strutManager
        edge: Unity2dPanel.LeftEdge
        widget: declarativeView
        height: launcherLoader.height
        width: launcherLoader.width
        enabled: launcherAlwaysVisible
    }

    PointerBarrier {
        property int x: declarativeView.globalPosition.x + (Utils.isLeftToRight() ? 0 : shell.width)
        property bool enableTrigger: launcherLoader.launcherInHideMode && launcherLoader.loadLauncher
        // This enableSticky logic has a 'bug' int RTL where the right most screen will still have enableSticky
        // set to true, but it won't cause any issue other than consuming a few bytes of mememory and calculating the 
        // rightmost x is probably more expensive and error prone than just enabling the extra barrier
        property bool enableSticky: unity2dConfiguration.stickyEdges && declarativeView.screen.geometry.x > 0

        id: leftBarrier
        triggerDirection: Utils.isLeftToRight() ? PointerBarrier.TriggerFromRight : PointerBarrier.TriggerFromLeft
        triggerZoneEnabled: enableTrigger && !launcherLoader.visibilityController.shown
        triggerOnly: enableTrigger && !enableSticky
        p1: Qt.point(x, declarativeView.screen.geometry.y)
        p2: Qt.point(x, declarativeView.screen.geometry.y + declarativeView.screen.geometry.height)
        triggerZoneP1: Qt.point(x, launcher2dConfiguration.revealMode == 1 ? declarativeView.screen.geometry.y : declarativeView.globalPosition.y)
        triggerZoneP2: Qt.point(x, launcher2dConfiguration.revealMode == 1 ? declarativeView.screen.geometry.y + 1 : declarativeView.globalPosition.y + launcherLoader.height)
        threshold: (enableSticky || enableTrigger) ? launcher2dConfiguration.edgeStopVelocity : -1
        maxVelocityMultiplier: launcher2dConfiguration.edgeResponsiveness
        decayRate: launcher2dConfiguration.edgeDecayrate
        triggerPressure: launcher2dConfiguration.edgeRevealPressure
        breakPressure: launcher2dConfiguration.edgeOvercomePressure

        onTriggered: launcherLoader.item.barrierTriggered()
    }

    PointerBarrier {
        property bool enableSticky: unity2dConfiguration.stickyEdges && declarativeView.screen.geometry.y > 0

        id: topBarrier
        p1: Qt.point(declarativeView.screen.geometry.x, declarativeView.screen.geometry.y)
        p2: Qt.point(declarativeView.screen.geometry.x + declarativeView.screen.geometry.width, declarativeView.screen.geometry.y)
        threshold: enableSticky ? launcher2dConfiguration.edgeStopVelocity : -1
        maxVelocityMultiplier: launcher2dConfiguration.edgeResponsiveness
        decayRate: launcher2dConfiguration.edgeDecayrate
        breakPressure: launcher2dConfiguration.edgeOvercomePressure
    }
}
