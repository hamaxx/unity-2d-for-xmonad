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

    Item {
        id: header

        Accessible.name: u2d.tr(filterModel.name)

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

            anchors.top: parent.top
            anchors.topMargin: isFirst ? 0 : 13
            anchors.left: parent.left

            text: u2d.tr(filterModel.name)
            font.pixelSize: 16
            font.bold: true
            font.underline: ( parent.state == "selected" || parent.state == "hovered" )
        }

        /* FIXME: add an "all" button
           filter.filtering is a bool indicating its state
           filter.clear() is the method that should be used when clicking on it */
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
