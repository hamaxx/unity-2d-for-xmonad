/*
 * This file is part of unity-2d
 *
 * Copyright 2011 Canonical Ltd.
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

RendererGrid {
    cellWidth: 280
    cellHeight: 75
    horizontalSpacing: 10
    verticalSpacing: 10

    cellRenderer: Component {
        AbstractButton {
            id: button

            property string uri
            property string iconHint
            property string mimetype
            property string displayName
            property string comment
            property string dndUri

            Accessible.name: displayName

            onClicked: {
                dashView.active = false
                lens.activate(decodeURIComponent(uri))
            }

            DragItemWithUrl {
                anchors.fill: parent
                url: decodeURIComponent(dndUri)
                defaultAction: {
                    if (!url.indexOf("application://")) return Qt.CopyAction
                    else if (!url.indexOf("unity-install://")) return Qt.IgnoreAction
                    else return Qt.LinkAction
                }
                supportedActions: defaultAction
                delegate: Component {
                    Image {
                        source: icon.source
                        width: icon.width
                        height: icon.height
                        fillMode: icon.fillMode
                        sourceSize.width: width
                        sourceSize.height: height
                        asynchronous: true
                    }
                }
                onPressed: parent.pressed = true
                onReleased: {
                    parent.pressed = false
                    parent.clicked()
                }
                onDrop: parent.pressed = false
            }

            ButtonBackground {
                anchors.fill: parent
                state: button.state
            }

            Image {
                id: icon

                /* Heuristic: if iconHint does not contain a '/' then it is an icon name */
                source: iconHint != "" && iconHint.indexOf("/") == -1 ? "image://icons/" + iconHint : iconHint
                width: 48
                height: 48
                anchors.verticalCenter: parent.verticalCenter
                anchors.left: parent.left
                anchors.leftMargin: 3
                fillMode: Image.PreserveAspectFit
                sourceSize.width: width
                sourceSize.height: height

                asynchronous: true
                opacity: status == Image.Ready ? 1 : 0
                Behavior on opacity {NumberAnimation {duration: 200; easing.type: Easing.InOutQuad}}
            }

            Item {
                id: labels

                anchors.top: parent.top
                anchors.topMargin: 3
                anchors.bottom: parent.bottom
                anchors.bottomMargin: 3
                anchors.left: icon.right
                anchors.leftMargin: 15
                anchors.right: parent.right
                anchors.rightMargin: 3

                TextCustom {
                    id: firstLine

                    text: displayName
                    color: button.state == "pressed" ? "#5e5e5e" : "#ffffff"
                    elide: Text.ElideMiddle

                    anchors.left: parent.left
                    anchors.right: parent.right
                    anchors.top: parent.top
                    height: paintedHeight
                }

                TextCustom {
                    id: secondLine

                    text: comment
                    color: button.state == "pressed" ? "#888888" : "#cccccc"

                    anchors.left: parent.left
                    anchors.right: parent.right
                    anchors.top: firstLine.bottom
                    anchors.bottom: parent.bottom

                    clip: true
                    wrapMode: Text.Wrap
                    verticalAlignment: Text.AlignTop
                }
            }
        }
    }
}
