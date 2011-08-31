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
import Unity2d 1.0

FocusScope {
    /* declare width & spacing of icons as required for layout calculations */
    property int iconWidth: 24
    property int iconSpacing: 36

    property variant visibleLenses: SortFilterProxyModel {
        model: dash.lenses
        dynamicSortFilter: true

        filterRole: Lenses.RoleVisible
        filterRegExp: RegExp("^true$")
    }

    Rectangle {
        id: background

        anchors.fill: parent
        color: "black"
        opacity: 0.22
    }

    /* LensBar contains a row of LensButtons */
    Row {
        id: lensContainer

        anchors.horizontalCenter: background.horizontalCenter
        anchors.top: background.top
        anchors.bottom: background.bottom
        spacing: iconSpacing

        Keys.onPressed: if (handleKeyPress(event.key)) event.accepted = true

        /* The Home lens is unfortunately not supplied by the "lenses" list
           This causes the keyboard navigation logic to be messy */
        property int currentIndex: 0

        function selectChild(index) {
            var child = lensContainer.childFromIndex(index)
            if (child != undefined) {
                child.focus = true
                currentIndex = index
                return true
            } else {
                return false
            }
        }

        function handleKeyPress(key) {
            switch (key) {
            case Qt.Key_Right:
                return selectChild(currentIndex+1)
            case Qt.Key_Left:
                return selectChild(currentIndex-1)
            }
        }

        function childFromIndex(index) {
            var indexInChildren = 0
            for(var i=0; i<children.length; i++) {
                if (children[i] != repeater) {
                    if (indexInChildren == index) return children[i]
                    indexInChildren++
                }
            }
            return undefined
        }

        /* Need to manually include the Home lens */
        LensButton {
            id: homeLens

            focus: true
            icon: "artwork/lens-nav-home.svg"
            onClicked: dash.activateHome()
            active: ( dashView.activeLens == "" )
            width: iconWidth
            anchors.top: parent.top
            anchors.bottom: parent.bottom
        }

        /* Now fetch all other lenses and display */
        Repeater{
            id: repeater

            model: visibleLenses
            delegate: LensButton {
                /* Heuristic: if iconHint does not contain a '/' then it is an icon name */
                icon: item.iconHint.indexOf("/") == -1 ? "image://icons/" + item.iconHint : item.iconHint
                active: item.active
                onClicked: dash.activateLens(item.id)
                width: iconWidth
                anchors.top: parent != undefined ? parent.top : undefined
                anchors.bottom: parent != undefined ? parent.bottom : undefined
            }
        }
    }
}
