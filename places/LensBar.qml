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
    //declare width & spacing of icons as required for layout calculations
    property int iconWidth: 32
    property int iconSpacing: 28

    function activateLens( place ) {
        /* FIXME: PlaceEntry.SetActiveSection needs to be called after
           PlaceEntry.SetActive in order for it to have an effect.
           This is likely a bug in the place daemons.
        */
        place.active = true
        //place.activeSection = section
        pageLoader.source = "PlaceEntryView.qml"
        /* Take advantage of the fact that the loaded qml is local and setting
           the source loads it immediately making pageLoader.item valid */
        pageLoader.item.model = place
        activatePage(pageLoader.item)
        dashView.activePlaceEntry = place.dbusObjectPath
    }

    property variant lenses: SortFilterProxyModel {
        id: visiblePlaces
        model: places
        dynamicSortFilter: true

        filterRole: Place.RoleShowEntry
        filterRegExp: RegExp("^true$")
    }

    /* Lens Bar rectangle */
    Rectangle {
        id: lensBar
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        width: parent.width
        color: "black"
        opacity: 0.22
    }

    /* LensBar contains a row of LensButtons */
    Row {
        id: lensContainer
        anchors.horizontalCenter: lensBar.horizontalCenter
        anchors.horizontalCenterOffset: 2
        anchors.top: lensBar.top
        anchors.bottom: lensBar.bottom
        spacing: iconSpacing

        // The Home lens is unfortunately not supplied by the "lenses" list
        // This causes the keyboard navigation logic to be messy
        property int currentIndex: 0

        function selectChild(index) {
            if (index < 0 || index > lenses.count) return false
            if (index == 0){
                homeLens.focus = true
            }
            else{
                if(currentIndex == 0) lensList.focus = true
                lensList.currentIndex = index-1
            }
            currentIndex = index
            return true
        }

        Keys.onPressed: if (handleKeyPress(event.key)) event.accepted = true
        function handleKeyPress(key) {
            switch (key) {
            case Qt.Key_Right:
                return selectChild(currentIndex+1)
            case Qt.Key_Left:
                return selectChild(currentIndex-1)
            }
        }

        //Need to manually include the Home lens
        LensButton {
            id: homeLens
            focus: true
            icon: "artwork/home.png"
            onClicked: activateHome()
            active: ( dashView.activePlaceEntry == "" )
        }

        //Now fetch all other lenses and display
        Component {
            id: lensDelegate

            LensButton {
                icon: item.icon
                active: item.active
                onClicked: activateLens(item)
            }
        }

        ListView {
            id: lensList
            height: lensBar.height
            width: lenses.count*iconWidth + (lenses.count-1)*iconSpacing
            model: lenses
            delegate: lensDelegate
            orientation: "Horizontal"
            spacing: iconSpacing
            interactive: false
        }
    }
}
