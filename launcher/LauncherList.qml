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
import Unity2d 1.0 /* required for drag’n’drop handling */

AutoScrollingListView {
    id: list

    /* The spacing is explicitly set to 0 and compensated for
       by adding some padding to the items because of
       http://bugreports.qt.nokia.com/browse/QTBUG-17622. */
    spacing: 0
    property int itemPadding: 5

    property int tileSize: 54

    /* Keep a reference to the currently visible contextual menu */
    property variant visibleMenu

    /* Can we reorder the items in this list by means of drag and drop ? */
    property alias reorderable: reorder.enabled

    /* z value used by items that want to be overlaid on top of the items of the list */
    property int overlayZ: list.contentItem.z + 3

    ListViewDragAndDrop {
        id: reorder
        list: list
        enabled: false
        z: overlayZ
    }

    clip: true

    /* Gradients overlaid on the items indicating that there are more items offscreen */
    Image {
        id: topGradient

        anchors.top: list.top
        /* Take into account the one pixel border of the background */
        width: list.width - 1
        height: Math.max(0, Math.min(list.contentY, 50))
        source: "artwork/gradient_more_items_top.png"
        z: overlayZ
    }

    Image {
        id: bottomGradient

        anchors.bottom: list.bottom
        width: topGradient.width
        height: Math.max(0, Math.min(list.contentHeight - list.contentY - list.height, 50))
        source: "artwork/gradient_more_items_bottom.png"
        z: overlayZ
    }

    /* FIXME: We need this only to workaround a problem in QT's MouseArea
       event handling. See AutoScrollingListView for details. */
    dragAndDrop: (reorder.enabled) ? reorder : null

    delegate: LauncherItem {
        id: launcherItem

        function accessibleDescription() {
            if (running) {
                var windows = u2d.tr("%1 window opened", "%1 windows opened", item.windowCount).arg(item.windowCount)
                return "%1 %2".arg(item.name).arg(windows)
            } else {
                return "%1 %2".arg(item.name).arg(u2d.tr("not running"))
            }
        }

        Accessible.name: accessibleDescription()

        width: list.width
        tileSize: list.tileSize
        padding: list.itemPadding

        desktopFile: item.desktop_file ? item.desktop_file : ""
        icon: item.icon != "" ? "image://icons/" + item.icon : "image://icons/unknown"
        running: item.running
        active: item.active
        urgent: item.urgent
        launching: item.launching
        pips: Math.min(item.windowCount, 3)

        counter: item.counter
        counterVisible: item.counterVisible
        progress: item.progress
        progressBarVisible: item.progressBarVisible
        emblem: item.emblem ? "image://icons/" + item.emblem : ""
        emblemVisible: item.emblemVisible

        /* Launcher of index 0 is the so-called BFB or Dash launcher */
        shortcutVisible: launcherView.superKeyHeld &&
                         ((item.toString().indexOf("LauncherApplication") == 0 && index > 0 && index <= 10) ||
                          item.shortcutKey != 0)
        shortcutText: {
            if (item.toString().indexOf("LauncherApplication") == 0) {
                return index % 10
            } else {
                return String.fromCharCode(item.shortcutKey).toLowerCase()
            }
        }

        isBeingDragged: (reorder.draggedTileId != "") && (reorder.draggedTileId == desktopFile)
        dragPosition: reorder.listCoordinates.y - list.contentY

        /* Best way I could find to check if the item is an application or the
           workspaces switcher. There may be something cleaner and better. */
        backgroundFromIcon: item.toString().indexOf("LauncherApplication") == 0 ||
                            item.toString().indexOf("Workspaces") == 0

        Binding { target: item.menu; property: "title"; value: item.name }

        /* Drag’n’drop handling */
        onDragEnter: item.onDragEnter(event)
        onDrop: item.onDrop(event)

        function showMenu() {
            /* Prevent the simultaneous display of multiple menus */
            if (list.visibleMenu != item.menu && list.visibleMenu != undefined) {
                list.visibleMenu.hide()
            }
            list.visibleMenu = item.menu
            // FIXME: The extra 2 pixels are needed to center the menu arrow with
            // the center of the tile.
            item.menu.show(width, panel.y + list.y - list.contentY +
                                  y + height - tileSize / 2 - 2)

        }

        onClicked: {
            if (mouse.button == Qt.LeftButton) {
                item.menu.hide()
                item.activate()
            }
            else if (mouse.button == Qt.RightButton) {
                /* Show the menu first, then unfold it. Doing things in this
                   order is required because at the moment the code path that
                   adjusts the position of the menu in case it goes offscreen
                   is traversed only when unfolding it.
                   See FIXME in LauncherContextualMenu::show(…). */
                showMenu()
                item.menu.folded = false
            }
            else if (mouse.button == Qt.MidButton) {
                item.launchNewInstance()
            }
        }

        /* Display the tooltip when hovering the item only when the list
           is not moving */
        onEntered: if (!list.moving && !list.autoScrolling) showMenu()
        onExited: {
            /* When unfolded, leave enough time for the user to reach the
               menu. Necessary because there is some void between the item
               and the menu. Also it fixes the case when the user
               overshoots. */
            if (!item.menu.folded)
                item.menu.hideWithDelay(400)
            else
                item.menu.hide()
        }

        Keys.onPressed: {
            if (event.key == Qt.Key_Return || event.key == Qt.Key_Enter || event.key == Qt.Key_Space) {
                item.menu.hide()
                item.activate()
                event.accepted = true
            }
            else if (event.key == Qt.Key_Right ||
                    (event.key == Qt.Key_F10 && (event.modifiers & Qt.ShiftModifier))) {
                /* Show the menu first, then unfold it. Doing things in this
                   order is required because at the moment the code path that
                   adjusts the position of the menu in case it goes offscreen
                   is traversed only when unfolding it.
                   See FIXME in LauncherContextualMenu::show(…). */
                showMenu()
                item.menu.folded = false
                item.menu.setFocus()
                event.accepted = true
            }
            else if (event.key == Qt.Key_Left) {
                item.menu.hide()
                event.accepted = true
            }
        }

        onActiveFocusChanged: {
            if (!activeFocus) {
                item.menu.hide()
            }
        }

        Connections {
            target: item.menu
            /* The menu had the keyboard focus because the launcher had
               activated it. Restore it. */
            onDismissedByKeyEvent: launcherView.forceActivateWindow()
        }

        Connections {
            target: list
            onMovementStarted: item.menu.hide()
            onAutoScrollingChanged: if (list.autoScrolling) item.menu.hide()
        }

        Connections {
            target: reorder
            /* Hide the tooltip/menu when dragging an application. */
            onDraggedTileIdChanged: if (reorder.draggedTileId != "") item.menu.hide()
        }

        Connections {
            target: item.menu
            onFoldedChanged: {
                if (item.menu.folded) {
                    visibilityController.endForceVisible();
                } else {
                    visibilityController.beginForceVisible();
                }
            }
        }

        function setIconGeometry() {
            if (running) {
                item.setIconGeometry(x + panel.x, y + panel.y, width, height)
            }
        }

        ListView.onAdd: SequentialAnimation {
            PropertyAction { target: launcherItem; property: "scale"; value: 0 }
            NumberAnimation { target: launcherItem; property: "height";
                              from: 0; duration: 250; easing.type: Easing.InOutQuad }
            NumberAnimation { target: launcherItem; property: "scale"; to: 1; duration: 250; easing.type: Easing.InOutQuad }
        }

        ListView.onRemove: SequentialAnimation {
            /* Disable all mouse interactions on the delegate being removed. This prevents a bug where QT itself
               crashes when trying to access some properties of the model item being removed while the item is being
               "kept alive" by ListView.delayRemove.
               In our case the property causing the bug is 'menu', but there may be others, so it's safer to just disable
               all mouse interactions. These interactions should not happen anyway while the tile is animating away
               since there's nothing that the user can do with it anyway: it's for all intents and purposes already gone.
               See: https://bugs.launchpad.net/unity-2d/+bug/719507 */
            PropertyAction { target: launcherItem; property: "interactive"; value: false }

            PropertyAction { target: launcherItem; property: "ListView.delayRemove"; value: true }
            NumberAnimation { target: launcherItem; property: "scale"; to: 0; duration: 250; easing.type: Easing.InOutQuad }
            NumberAnimation { target: launcherItem; property: "height"; to: 0; duration: 250; easing.type: Easing.InOutQuad }
            PropertyAction { target: launcherItem; property: "ListView.delayRemove"; value: false }
        }

        onRunningChanged: setIconGeometry()
        /* Note: this doesn’t work as expected for the first favorite
           application in the list if it is already running when the
           launcher is started, because its y property doesn’t change.
           This isn’t too bad though, as the launcher is supposed to be
           started before any other regular application. */
        onYChanged: setIconGeometry()

        Connections {
            target: item
            onWindowAdded: item.setIconGeometry(x + panel.x, y + panel.y, width, height, xid)
            /* Not all items are applications. */
            ignoreUnknownSignals: true
        }

        Connections {
            target: urgentAnimation
            onRunningChanged: {
                if (urgentAnimation.running) {
                    visibilityController.beginForceVisible()
                } else {
                    visibilityController.endForceVisible()
                }
            }
            Component.onCompleted: {
                if (urgentAnimation.running) {
                    visibilityController.beginForceVisible()
                }
            }
            Component.onDestruction: {
                if (urgentAnimation.running) {
                    visibilityController.endForceVisible()
                }
            }
        }

        Connections {
            target: launcherView
            onActivateShortcutPressed: {
                /* Only applications can be launched by keyboard shortcuts */
                if (item.toString().indexOf("LauncherApplication") == 0 && index == itemIndex) {
                    item.menu.hide()
                    item.activate()
                }
            }
            onNewInstanceShortcutPressed: {
                /* Only applications can be launched by keyboard shortcuts */
                if (item.toString().indexOf("LauncherApplication") == 0 && index == itemIndex) {
                    item.menu.hide()
                    item.launchNewInstance()
                }
            }
        }
    }
}
