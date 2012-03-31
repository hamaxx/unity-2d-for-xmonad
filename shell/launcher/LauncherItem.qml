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
import "../common"
import "../common/utils.js" as Utils

/* This component represents a single "tile" in the launcher and the surrounding
   indicator icons.

   The tile is square in size, with a side determined by the 'tileSize' property,
   and rounded borders.
   It is composed by a colored background layer, an icon (with 'icon' as source),
   and a layer on top that provides a "shine" effect.
   The main color of the background layer may be calculated based on the icon color
   or may be fixed (depending on the 'backgroundFromIcon' property).

   There's also an additional layer which contains only the outline of the tile
   that is only appearing during the launching animation (when the 'launching' property is
   true). During this animation the background fades out and the outline fades in,
   giving a "pulsing" appearance to the tile.

   Around the tile we may have on the left a number of "pips" between zero and three.
   Pips are small icons used to indicate how many windows we have open for the current tile
   (based on the 'windowCount' property).
   The rule is: if there's only one window, we just display an arrow. If there are
   two we display 2 pips, if there are 3 or more display 3 pips.

   On the right of the tile there's an arrow that appears if the tile is currently 'active'.

   Additionally, when the tile is marked as 'urgent' it will start an animation where the
   rotation is changed so that it appears to be "shaking".
*/
DropItem {
    id: item

    Accessible.role: Accessible.PushButton

    anchors.horizontalCenter: parent.horizontalCenter

    height: selectionOutlineSize

    property bool isBfb: false
    property int tileSize
    property int selectionOutlineSize
    property alias name: looseItem.objectName
    property string desktopFile: ""
    property string icon: "image://icons/unknown"
    property alias urgentAnimation: urgentAnimation
    property bool running: false
    property bool active: false
    property bool activeOnThisScreen: false
    property bool urgent: false
    property bool launching: false
    property alias interactive: mouse.enabled

    property int counter: 0
    property bool counterVisible: false
    property real progress: 0.0
    property bool progressBarVisible: false
    property alias emblem: emblemIcon.source
    property bool emblemVisible: false

    property bool backgroundFromIcon
    property color defaultBackgroundColor: "#333333"
    property color selectedBackgroundColor: "#dddddd"

    property alias shortcutVisible: shortcut.visible
    property alias shortcutText: shortcutText.text

    property bool beHudItem: shellManager.hudActive && shellManager.hudShell == declarativeView
                             && isBfb && launcher2dConfiguration.hideMode == 0

    property bool isBeingDragged: false
    property int dragPosition

    property int pips: 0
    property string pipSource: "launcher/artwork/launcher_" +
                               ((pips <= 1) ? "arrow" : "pip") + "_ltr.png"
    function getPipOffset(index) {
        /* Pips need to always be centered, regardless if they are an even or odd
           number. The following simple conditional code works and is less
           convoluted than a generic formula. It's ok since we always work with at
           most three pips anyway. */
        if (pips == 1) return 0
        if (pips == 2) return (index == 0) ? -2 : +2
        else return (index == 0) ? 0 : (index == 1) ? -4 : +4
    }

    signal clicked(variant mouse)
    signal pressed(variant mouse)
    signal entered
    signal exited

    Item {
        /* The actual item, reparented so its y coordinate can be animated. */
        id: looseItem
        LayoutMirroring.enabled: Utils.isRightToLeft()
        LayoutMirroring.childrenInherit: true
        parent: list
        width: item.width
        height: item.height
        x: item.x
        y: -item.ListView.view.contentY + item.y
        /* The item is above the list's contentItem.
           Top and bottom gradients, ListViewDragAndDrop and autoscroll areas
           are above the item */
        z: list.contentItem.z + 1

        /* Bind to the scale of the delegate so that it is animated upon insertion/removal */
        scale: item.scale

        /* The y coordinate is initially not animated, as it would result in an
           unwanted effect of every single item popping out from the top of the
           launcher (even when they are supposed to be coming from the bottom).
           This property is later set to true once the item has taken its
           initial position. */
        property bool animateY: false

        /* This is the arrow shown at the right of the tile when the application is
           the active one */
        Image {
            objectName: "active"
            anchors.right: parent.right
            y: item.height - item.selectionOutlineSize / 2 - height / 2
            mirror: Utils.isRightToLeft()

            source: "image://blended/%1color=%2alpha=%3"
                  .arg("launcher/artwork/launcher_arrow_" + (activeOnThisScreen || beHudItem ? "" : "outline_" ) + "rtl.png")
                  .arg("lightgrey")
                  .arg(1.0)

            visible: (active && (looseItem.state != "beingDragged")) || beHudItem
        }

        /* This is the area on the left of the tile where the pips/arrow end up.

           I'd rather use a Column here, but the pip images have an halo
           around them, so they are pretty tall and would mess up the column.
           As a workaround I center all of them, then shift up or down
           depending on the index. */
        Repeater {
            model: item.pips
            delegate: Image {
                objectName: "pips-" + index
                /* FIXME: It seems that when the image is created (or re-used) by the Repeater
                   for a moment it doesn't have any parent, and therefore warnings are
                   printed for the following two anchor assignements. This fixes the
                   problem, but I'm not sure if it should happen in the first place. */
                anchors.left: (parent) ? parent.left : undefined
                y: item.height - item.selectionOutlineSize / 2 - height / 2 + getPipOffset(index)
                mirror: Utils.isRightToLeft()

                source: "image://blended/%1color=%2alpha=%3"
                        .arg(pipSource).arg("lightgrey").arg(1.0)

                visible: looseItem.state != "beingDragged"
            }
        }

        MouseArea {
            id: mouse
            anchors.fill: parent
            hoverEnabled: true
            acceptedButtons: Qt.LeftButton | Qt.RightButton | Qt.MidButton

            onClicked: item.clicked(mouse)
            onEntered: item.entered()
            onExited: item.exited()
            onPressed: item.pressed(mouse)
        }

        /* This is the for centering the actual tile in the launcher */
        Item {
            id: tile
            width: item.tileSize
            height: item.tileSize
            anchors.centerIn: parent

            /* This draws the icon, the tile background and the sheen on top */
            IconTile {
                id: icon
                width: item.tileSize
                height: item.tileSize
                anchors.centerIn: parent

                activeFocus: item.activeFocus
                backgroundFromIcon: item.backgroundFromIcon

                source: (beHudItem && hudLoader) ? hudLoader.item.appIcon : item.icon
                tileBackgroundImage: (item.isBfb) ? "../launcher/artwork/squircle_base_54.png" : ""
                tileShineImage: (item.isBfb) ? "../launcher/artwork/squircle_shine_54.png" : ""
                selectedTileBackgroundImage: (item.isBfb) ? "../launcher/artwork/squircle_base_selected_54.png" : ""

                /* tile background fade in/out animation */
                SequentialAnimation on backgroundOpacity {
                    NumberAnimation { to: 0.0; duration: 1000; easing.type: Easing.InOutQuad }
                    NumberAnimation { to: 1.0; duration: 1000; easing.type: Easing.InOutQuad }

                    loops: Animation.Infinite
                    alwaysRunToEnd: true
                    running: launching
                }
            }

            /* This image appears only while launching, and pulses in and out in counterpoint
               to the background, so that the outline of the tile is always visible. */
            Image {
                id: tileOutline
                anchors.fill: parent
                smooth: true

                sourceSize.width: item.tileSize
                sourceSize.height: item.tileSize
                source: "artwork/round_outline_54x54.png"

                opacity: 0

                SequentialAnimation on opacity {
                    NumberAnimation { to: 1.0; duration: 1000; easing.type: Easing.InOutQuad }
                    NumberAnimation { to: 0.0; duration: 1000; easing.type: Easing.InOutQuad }

                    loops: Animation.Infinite
                    alwaysRunToEnd: true
                    running: launching
                }
            }

            Image {
                id: selectionOutline
                objectName: "selectionOutline"
                anchors.centerIn: parent
                smooth: true
                source: isBfb ? "artwork/squircle_glow_54.png" : "artwork/round_selected_66x66.png"
                visible: declarativeView.focus && item.activeFocus
            }

            Rectangle {
                id: counter
                height: 16 - border.width
                width: 32
                // Using anchors the item will be 1 pixel off with respect to Unity
                y: 1
                x: 1
                radius: height / 2 - 1
                smooth: true
                border.width: 2
                border.color: "white"
                color: "#595959"
                visible: launcherItem.counterVisible

                Text {
                    anchors.centerIn: parent
                    font.pixelSize: parent.height - 3
                    width: parent.width - 5
                    smooth: true
                    elide: Text.ElideRight
                    horizontalAlignment: Text.AlignHCenter
                    color: "white"
                    text: launcherItem.counter
                }
            }

            Image {
                id: progressBar
                objectName: "progressBar"
                source: "artwork/progress_bar_trough.png"
                anchors.verticalCenter: parent.verticalCenter
                anchors.left: parent.left
                width: tile.width
                smooth: true
                state: launcherItem.progressBarVisible ? "" : "hidden"

                Image {
                    id: progressFill
                    source: "artwork/progress_bar_fill.png"
                    anchors.verticalCenter: parent.verticalCenter
                    x: 6
                    width: sourceSize.width * launcherItem.progress
                    smooth: true

                    Behavior on width {
                       NumberAnimation { duration: 200; easing.type: Easing.InOutSine }
                    }
                }

                Behavior on width {
                    NumberAnimation { duration: 200; easing.type: Easing.InOutSine }
                }

                states: State {
                    name: "hidden"
                    PropertyChanges {
                        target: progressBar
                        width: 0
                    }
                    // This, combined with anchors.left: parent.left in the default state
                    // makes the bar seem to come in from the left and go away at the right
                    AnchorChanges {
                        target: progressBar
                        anchors.left: undefined
                        anchors.right: tile.right
                    }
                }
            }

            Rectangle {
                id: shortcut
                anchors.centerIn: parent
                color: "#B3000000" // 0.7 opacity on black
                radius: 2
                width: 22
                height: 22
                smooth: true

                Text {
                    id: shortcutText
                    anchors.centerIn: parent
                    color: "white"
                    smooth: true
                }
            }

            Image {
                id: emblemIcon
                anchors.left: parent.left
                anchors.top: parent.top
                visible: launcherItem.emblemVisible && !counter.visible
                smooth: true
            }


            /* The entire tile will "shake" when the window is marked as "urgent", to attract
               the user's attention */
            SequentialAnimation {
                id: urgentAnimation
                running: urgent
                alwaysRunToEnd: true

                SequentialAnimation {
                    loops: (urgent) ? 30 : 0
                    NumberAnimation { target: tile; property: "rotation"; to: 15; duration: 150 }
                    NumberAnimation { target: tile; property: "rotation"; to: -15; duration: 150 }
                }
                NumberAnimation { target: tile; property: "rotation"; to: 0; duration: 75 }
            }
        }

        states: State {
            name: "beingDragged"
            when: item.isBeingDragged
            PropertyChanges {
                target: looseItem
                y: item.dragPosition - tile.height / 2
                /* When dragging an item, stack it on top of all its siblings */
                z: list.contentItem.z + 2
            }
        }
        Behavior on y {
            enabled: /* do not animate during initial positioning */
                     looseItem.animateY
                     /* do not animate while dragging to re-order applications */
                     && (looseItem.state != "beingDragged")
                     /* do not animate during insertion/removal */
                     && (looseItem.scale == 1)
                     /* do not animate while flicking the list */
                     && !item.ListView.view.moving
                     && !item.ListView.view.autoScrolling
            NumberAnimation {
                duration: 250
                easing.type: Easing.OutBack
            }
        }

        /* Delay the animation on y to when the item has been initially positioned. */
        Timer {
            id: canAnimateY
            /* This ensures that the trigger will be executed in the next
               iteration of the event loop, at which point the item will have
               taken its initial position. */
            triggeredOnStart: true
            onTriggered: {
                stop()
                looseItem.animateY = true
            }
        }
        Component.onCompleted: canAnimateY.start()
    }
}
