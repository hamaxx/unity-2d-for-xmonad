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

FocusScope {
    property alias scrollbar: scrollbar
    property alias model: list.model
    property alias bodyDelegate: list.bodyDelegate
    property alias headerDelegate: list.headerDelegate

    ListViewWithHeaders {
        id: list

        focus: true
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.rightMargin: 15
    }

    Scrollbar {
        id: scrollbar

        anchors.top: parent.top
        anchors.topMargin: 15
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 10
        anchors.right: parent.right

        targetFlickable: list.flickable

        /* Hide the scrollbar if there is less than a page of results */
        opacity: targetFlickable.visibleArea.heightRatio < 1.0 ? 1.0 : 0.0
        Behavior on opacity {NumberAnimation {duration: 100}}
    }
}
