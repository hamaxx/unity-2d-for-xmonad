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
import "utils.js" as Utils

FocusScope {
    id: filterLoader

    property variant lens
    property variant filterModel
    property bool isFirst

    height: childrenRect.height

    FocusScope {
        id: header

        function accessibleDescription() {
            if (filterModel.filtering) {
                return u2d.tr("Filter %1 active").arg(filterModel.name)
            } else {
                return u2d.tr("Filter %1 inactive").arg(filterModel.name)
            }
        }

        Accessible.name: accessibleDescription()

        KeyNavigation.down: filterView

        focus: true
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        height: childrenRect.height

        Rectangle {
            id: separatorLine

            color: "#21ffffff"

            height: 1
            visible: !isFirst
            anchors.top: parent.top
            anchors.left: parent.left
            anchors.right: parent.right
        }

        TextCustom {
            id: title

            anchors.verticalCenter: clearButton.verticalCenter
            anchors.topMargin: isFirst ? 0 : 13
            anchors.left: parent.left

            text: u2d.tr(filterModel.name)
            fontSize: "large"
            font.bold: true
            font.underline: ( parent.state == "selected" || parent.state == "hovered" )
        }

        TickBox {
            id: clearButton

            focus: true

            // FIXME: that will probably break with non-latin LANG
            // dynamically calculate button width, but limit to 100
            width: Math.min(100, (10 * u2d.tr("All").length) + 18)
            height: 30
            anchors.top: parent.top
            anchors.topMargin: isFirst ? 0 : 13
            anchors.right: parent.right

            text: u2d.tr("All")
            checked: !filterModel.filtering
            onClicked: filterModel.clear()
        }
    }

    Loader {
        id: filterView

        KeyNavigation.up: header
        anchors.top: header.bottom
        anchors.topMargin: 15
        anchors.left: parent.left
        anchors.right: parent.right

        source: Utils.convertToCamelCase(filterModel.rendererName) + ".qml"
        onStatusChanged: {
            if (status == Loader.Error) {
                console.log("Failed to load filter renderer", filterModel.rendererName)
            }
        }

        Binding { target: filterView.item; property: "lens"; value: filterLoader.lens }
        Binding { target: filterView.item; property: "filterModel"; value: filterLoader.filterModel }

        onLoaded: item.focus = true
    }
}
