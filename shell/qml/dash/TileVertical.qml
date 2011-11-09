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

RendererGrid {
    cellWidth: 100
    cellHeight: 112
    horizontalSpacing: 42
    verticalSpacing: 20

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
                anchors.fill: icon
                anchors.margins: -4
                state: button.state
            }

            Image {
                id: icon

                source: iconHint != "" ? "image://icons/"+iconHint : "image://icons/unknown"
                onStatusChanged: if (status == Image.Error) source = "image://icons/unknown"
                width: 64
                height: 64
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.top: parent.top
                anchors.topMargin: 4
                fillMode: Image.PreserveAspectFit
                sourceSize.width: width
                sourceSize.height: height

                asynchronous: true
                opacity: status == Image.Ready ? 1 : 0
                Behavior on opacity {NumberAnimation {duration: 200; easing.type: Easing.InOutQuad}}
            }

            TextMultiLine {
                id: label

                text: displayName
                color: "#ffffff"
                state: ( parent.state == "selected" || parent.state == "hovered" ) ? "expanded" : ""
                horizontalAlignment: Text.AlignHCenter
                anchors.top: icon.bottom
                anchors.bottom: parent.bottom
                anchors.right: parent.right
                anchors.left: parent.left
                anchors.topMargin: 10
                anchors.rightMargin: 3
                anchors.leftMargin: 3
                fontSize: "small"
            }
        }
    }
}
