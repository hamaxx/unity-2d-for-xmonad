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

import QtQuick 1.0
import Unity2d 1.0
import "utils.js" as Utils

Rectangle {
    id: switcher

    color: "black"

    signal cancelAndExitStarted ()

    property variant declarativeView

    property int columns: desktop.workspaces.columns
    property int rows: desktop.workspaces.rows

    property int margin: 35
    property int spacing: 4

    /* Effective area available for laying out the workspaces after considering
       inter-workspace spaces */
    property int availableWidth: switcher.width - (columns * spacing)
    property int availableHeight: switcher.height - (rows * spacing)

    property int maxCellWidth: Math.floor(availableWidth / columns)
    property int maxCellHeight: Math.floor(availableHeight / rows)
    /* Depending on the aspect ratio of the final workspaces layout, we will
       have either vertical or horizontal margins. That is, we will either:
       - use maxCellWidth as base cell width and compute the height based on the
         screen ratio, or
       - use maxCellHeight for the base cell height and compute the width based
         on the screen ratio.
       To figure out which way to go, compute the other size and see how things
       would fit inside the screen.  */
    property int computedCellHeight: maxCellWidth * switcher.height / switcher.width
    property int computedCellWidth: maxCellHeight * switcher.width / switcher.height

    property bool useWidth: (computedCellHeight + spacing) * rows <= switcher.height

    property int cellWidth: useWidth ? maxCellWidth : computedCellWidth
    property int cellHeight: useWidth ? computedCellHeight : maxCellHeight
    property real cellScale: cellWidth / switcher.width

    /* Scale of a workspace when the user zooms on it (fills most of the switcher, leaving a margin to see
       the corners of the other workspaces below it) */
    property bool isDesktopHorizontal: declarativeView.screen.panelsFreeGeometry.width > declarativeView.screen.panelsFreeGeometry.height
    property real zoomedScale: (isDesktopHorizontal) ? ((width - 2*margin) / switcher.width) :
                                                       ((height - 2*margin) / switcher.height)

    /* When this is set, it is used to filter the global list of windows to limit it to
       a single application. See the SortFilterProxyModel used in Spread.qml */
    property string applicationFilter

    /* The number of the currently zoomed workspace (-1 means no workspace)
       Use workspaceForNumber to find the actual Workspace component. */
    property int zoomedWorkspace: -1

    /* This is the master model containing all the windows recognized by bamf for the entire
       screen. Each workspace will individually filter them to select only those that
       belong to it. */
    property variant allWindows: WindowsList { }

    /* Group all Workspace elements into a single Item to help workspaceByNumber
       iterate over less items than it would need to if the Repeater was adding children
       to the switcher itself. */
    GridView {
        id: workspaces
        anchors.centerIn: parent

        width: cellWidth * columns
        height: cellHeight * rows

        model: desktop.workspaces.count
        cellWidth: parent.cellWidth + spacing
        cellHeight: parent.cellHeight + spacing
        keyNavigationWraps: true
        property string windowFocus
        Keys.onPressed: {
            if (event.key == Qt.Key_Left || event.key == Qt.Key_Up) {
                windowFocus = "last"
            } else if (event.key == Qt.Key_Right || event.key == Qt.Key_Down) {
                windowFocus = "first"
            }
        }

        highlight: Rectangle {
            color: "orange"
            x: workspaces.currentItem.x
            y: workspaces.currentItem.y
            z: -1
            width: workspaces.cellWidth
            height: workspaces.cellHeight
            visible: workspaces.currentItem.state == "unzoomed" && spreadManager.currentSwitcher == switcher
        }
        highlightFollowsCurrentItem: false

        delegate: Workspace {
            id: workspace

            property int workspaceNumber: index

            width: workspaces.cellWidth
            height: workspaces.cellHeight

            unzoomedScale:  switcher.cellScale

            /* Center the workspace in 'zoomed' state */
            zoomedX: switcher.width * (1 - zoomedScale) / 2
            zoomedY: switcher.height * (1 - zoomedScale) / 2
            zoomedScale: switcher.zoomedScale

            Connections {
                target: workspaces
                onCurrentIndexChanged: {
                    if (workspaces.windowFocus == "first") {
                        setFocusOnFirstWindow()
                    } else if (workspaces.windowFocus == "last") {
                        setFocusOnLastWindow()
                    }
                }
            }
            state: {
                if (initial) {
                    if (desktop.workspaces.current == workspaceNumber) {
                        return "screen"
                    } else {
                        return "unzoomed"
                    }
                } else {
                    if (zoomedWorkspace == workspaceNumber) {
                        return "zoomed"
                    } else {
                        return "unzoomed"
                    }
                }
            }

            onEntered: {
                if (zoomedWorkspace == -1) {
                    workspaces.currentIndex = index
                }
            }

            onClicked: {
                if (zoomedWorkspace == workspaceNumber) {
                    activateWorkspace(workspaceNumber)
                } else if (zoomedWorkspace == -1) {
                    if (windowCount <= 1) {
                        activateWorkspace(workspaceNumber)
                    } else {
                        workspaces.currentIndex = index
                        zoomedWorkspace = workspaceNumber
                    }
                } else {
                    workspaces.currentIndex = index
                    zoomedWorkspace = -1
                }
            }
        }

        onCurrentIndexChanged: {
            zoomedWorkspace = -1
        }
    }

    /* FIXME: bad naming. Ideas: screenModeActivated, initialState */
    property bool initial: true

    /* This connection receives all commands from the DBUS API */
    Connections {
        target: control

        onShowCurrentWorkspace: {
            /* Setup application pre-filtering and initially zoomed desktop, if any
               were specified as arguments */
            applicationFilter = applicationDesktopFile
            zoomedWorkspace = desktop.workspaces.current
            show()
        }

        onShowAllWorkspaces: {
            if (desktop.workspaces.count > 1) {
                applicationFilter = applicationDesktopFile
                zoomedWorkspace = -1
                show()
            } else {
                control.showCurrentWorkspace(applicationDesktopFile)
            }
        }

        onHide: cancelAndExit()

        onFilterByApplication: applicationFilter = applicationDesktopFile
    }

    function show() {
        allWindows.load()

        declarativeView.show()
        workspaces.currentIndex = desktop.workspaces.current
        /* This is necessary otherwise we don't get keypresses until the user does a
           mouse over on a window */
        workspaces.forceActiveFocus()
        initial = false
    }

    /* This controls the exit from the switcher.
       Note that we can't just hide the workspaces switcher immediately when the user
       wants to activate a window or cancel the switching process. We first want any
       transitions to complete, so we need to start this timer, and when it's triggered
       it will actually do all that is necessary to hide the switcher and cleanup */
    Timer {
        id: exitTransitionTimer
        interval: Utils.transitionDuration
        onTriggered: {
            declarativeView.hide()

            /* Nothing should be allowed to touch the windows anymore here, so it should
               be safe to unload them all to save memory.
               NOTE: i'm not exactly sure any memory will actually be saved since the biggest
               usage is the window screenshots, and Qt is caching them (see SpreadWindow.qml
               for the trick I use to force them to refresh and more info on this cache)
            */
            allWindows.unload()
            zoomedWorkspace = -1
        }
    }


    /* Handle both the ESC keypress and any click on the area outside of the
       switcher in the same way: maximize the currently active workspace to screen
       size and hide the switcher (effectively canceling the switching operation).
       If another workspace was zoomed unzoom it first. */
    Keys.onPressed: {
        switch (event.key) {
        case Qt.Key_Escape:
            cancelAndExit()
            event.accepted = true
            return
        case Qt.Key_S:
            if (event.modifiers & Qt.MetaModifier) {
                cancelAndExit()
                event.accepted = true
                return
            }
            break
        }
    }

    Connections {
        target: spreadManager
        onStartCancelAndExit: {
            cancelAndExit(true)
        }
    }

    function cancelAndExit(fromSpreadManager) {
        if (!fromSpreadManager) {
            cancelAndExitStarted()
        }
        initial = true

        /* Let the transition finish and then hide the switcher and perform cleanup */
        exitTransitionTimer.start()
    }

    function activateWindow(windowInfo) {
        desktop.workspaces.changeCurrent(zoomedWorkspace)
        windowInfo.activate()
        cancelAndExit()
    }

    function activateWorkspace(workspaceNumber) {
        desktop.workspaces.changeCurrent(workspaceNumber)
        cancelAndExit()
    }
}
