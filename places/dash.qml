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
import Unity2d 1.0 /* Necessary for GnomeBackground and LauncherPlacesList*/

Item {
    id: dash

    property variant currentPage

    Binding {
        target: dashView
        property: "expanded"
        value: (currentPage && currentPage.expanded != undefined) ? currentPage.expanded : true
    }

    function activatePage(page) {
        /* Always give the focus to the search entry when switching pages */
        search_entry.focus = true

        if (page == currentPage) {
            return
        }

        if (currentPage != undefined) {
            currentPage.visible = false
        }
        currentPage = page
        currentPage.visible = true
    }

    function activatePlaceEntry(fileName, groupName, section) {
        var placeEntryModel = places.findPlaceEntry(fileName, groupName)
        if (placeEntryModel == null) {
            console.log("No match for place: %1 [Entry:%2]".arg(fileName).arg(groupName))
            return
        }

        /* FIXME: PlaceEntry.SetActiveSection needs to be called after
           PlaceEntry.SetActive in order for it to have an effect.
           This is likely a bug in the place daemons.
        */
        placeEntryModel.active = true
        placeEntryModel.activeSection = section
        pageLoader.source = "PlaceEntryView.qml"
        /* Take advantage of the fact that the loaded qml is local and setting
           the source loads it immediately making pageLoader.item valid */
        pageLoader.item.model = placeEntryModel
        activatePage(pageLoader.item)
        dashView.activePlaceEntry = placeEntryModel.dbusObjectPath
    }

    function activateHome() {
        pageLoader.source = "Home.qml"
        /* Take advantage of the fact that the loaded qml is local and setting
           the source loads it immediately making pageLoader.item valid */
        activatePage(pageLoader.item)
        dashView.activePlaceEntry = ""
    }

    property variant places: LauncherPlacesList {
        Component.onCompleted: startAllPlaceServices()
    }

    /* Backgrounds */
    GnomeBackground {
        anchors.fill: parent
        overlay_color: "black"
        overlay_alpha: 0.89
        visible: dashView.dashMode == DashDeclarativeView.FullScreenMode && !screen.isCompositingManagerRunning
    }

    Rectangle {
        anchors.fill: parent
        color: "black"
        opacity: 0.89
        visible: dashView.dashMode == DashDeclarativeView.FullScreenMode && screen.isCompositingManagerRunning
    }

    BorderImage {
        /* Avoid redraw at rendering necessary to prevent high CPU usage.
           Ref.: https://bugs.launchpad.net/unity-2d/+bug/806122
        */
        effect: CacheEffect {}

        anchors.fill: parent
        visible: dashView.dashMode == DashDeclarativeView.DesktopMode
        source: screen.isCompositingManagerRunning ? "artwork/desktop_dash_background.sci" : "artwork/desktop_dash_background_no_transparency.sci"
    }
    /* /Backgrounds */

    Item {
        anchors.fill: parent
        /* Margins in DesktopMode set so that the content does not overlap with
           the border defined by the background image.
        */
        anchors.bottomMargin: dashView.dashMode == DashDeclarativeView.DesktopMode ? 39 : 0
        anchors.rightMargin: dashView.dashMode == DashDeclarativeView.DesktopMode ? 37 : 0

        visible: dashView.active

        /* Unhandled keys will always be forwarded to the search bar. That way
           the user can type and search from anywhere in the interface without
           necessarily focusing the search bar first. */
        //Keys.forwardTo: [search_entry]


        SearchEntry {
            id: search_entry

            focus: true
            /* FIXME: check on visible necessary; fixed in Qt Quick 1.1
                      ref: http://bugreports.qt.nokia.com/browse/QTBUG-15862
            */
            KeyNavigation.right: refine_search.visible ? refine_search : search_entry
            KeyNavigation.down: pageLoader

            anchors.top: parent.top
            anchors.topMargin: 10
            anchors.left: parent.left
            anchors.leftMargin: 16
            anchors.right: refine_search.left
            anchors.rightMargin: 10

            height: 53
        }

        SearchRefine {
            id: refine_search

            KeyNavigation.left: search_entry

            /* SearchRefine is only to be displayed for places, not in the home page */
            visible: dashView.activePlaceEntry != ""
            placeEntryModel: visible && currentPage != undefined ? currentPage.model : undefined

            anchors.top: search_entry.anchors.top
            anchors.topMargin: search_entry.anchors.topMargin
            height: parent.height
            headerHeight: search_entry.height
            width: 295
            anchors.right: parent.right
            anchors.rightMargin: 19
        }

        Loader {
            id: pageLoader

            /* FIXME: check on visible necessary; fixed in Qt Quick 1.1
                      ref: http://bugreports.qt.nokia.com/browse/QTBUG-15862
            */
            KeyNavigation.right: refine_search.visible && !refine_search.folded ? refine_search : pageLoader
            KeyNavigation.up: search_entry

            anchors.top: search_entry.bottom
            anchors.topMargin: 2
            anchors.bottom: parent.bottom
            anchors.left: parent.left
            anchors.leftMargin: 20
            anchors.right: !refine_search.visible || refine_search.folded ? parent.right : refine_search.left
            anchors.rightMargin: !refine_search.visible || refine_search.folded ? 0 : 15
            onLoaded: item.focus = true
        }
    }

    Button {
        id: fullScreenButton
        anchors.bottom: parent.bottom
        anchors.right: parent.right
        anchors.rightMargin: 15
        anchors.bottomMargin: 15
        width: fullScreenButtonImage.sourceSize.width
        height: fullScreenButtonImage.sourceSize.height
        visible: dashView.dashMode != DashDeclarativeView.FullScreenMode

        Image {
            id: fullScreenButtonImage
            source: "artwork/fullscreen_button.png"
        }

        onClicked: {
            dashView.dashMode = DashDeclarativeView.FullScreenMode
        }
    }
}
