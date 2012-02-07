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
import Effects 1.0

Item {
    id: dash

    Accessible.name: "root"
    LayoutMirroring.enabled: isRightToLeft()
    LayoutMirroring.childrenInherit: true

    property variant currentPage
    property variant queuedLensId

    function isRightToLeft() {
        return Qt.application.layoutDirection == Qt.RightToLeft
    }

    Binding {
        target: dashView
        property: "expanded"
        value: (currentPage && currentPage.expanded != undefined) ? currentPage.expanded : true
    }

    Binding {
        target: dashView
        property: "dashMode"
        value: dashClient.alwaysFullScreen || dash2dConfiguration.fullScreen ?
               DashDeclarativeView.FullScreenMode : DashDeclarativeView.DesktopMode
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

    function buildLensPage(lens) {
        pageLoader.source = "LensView.qml"
        /* Take advantage of the fact that the loaded qml is local and setting
           the source loads it immediately making pageLoader.item valid */
        pageLoader.item.model = lens
        activatePage(pageLoader.item)
    }

    /* Set all Lenses as Hidden when Dash closes */
    function deactivateAllLenses() {
        for (var i=0; i<lenses.rowCount(); i++) {
            lenses.get(i).viewType = Lens.Hidden
        }
    }

    function activateLens(lensId) {
        /* check if lenses variable was populated already */
        if (lenses.rowCount() == 0) {
            queuedLensId = lensId
            return
        }

        var lens = lenses.get(lensId)
        if (lens == null) {
            console.log("No match for lens: %1".arg(lensId))
            return
        }

        if (lensId == dashView.activeLens) {
            /* we don't need to activate the lens, just show its UI */
            buildLensPage(lens)
            return
        }

        /* To activate lens, we set its viewType to LensView, and then set all
           other lenses to Hidden */
        for (var i=0; i<lenses.rowCount(); i++) {
            var thislens = lenses.get(i)
            thislens.viewType = (lens == thislens) ?  Lens.LensView : Lens.Hidden
        }

        buildLensPage(lens)
        dashView.activeLens = lens.id
    }

    function activateHome() {
        /* When Home is shown, need to notify all other lenses. Those in the global view
           (in home search results page) are set to HomeView, all others to Hidden */
        for (var i=0; i<lenses.rowCount(); i++) {
            var thislens = lenses.get(i)
            thislens.viewType = (thislens.searchInGlobal) ? Lens.HomeView : Lens.Hidden
        }

        pageLoader.source = "Home.qml"
        /* Take advantage of the fact that the loaded qml is local and setting
           the source loads it immediately making pageLoader.item valid */
        activatePage(pageLoader.item)
        dashView.activeLens = ""
    }

    function activateLensWithOptionFilter(lensId, filterId, optionId) {
        var lens = lenses.get(lensId)
        var filter = lens.filters.getFilter(filterId)
        var option = filter.getOption(optionId)
        filter.clear()
        option.active = true
        filterPane.folded = false
        activateLens(lensId)
    }

    function activateLensAndClearFilter(lensId, filterId) {
        var lens = lenses.get(lensId)
        var filter = lens.filters.getFilter(filterId)
        filter.clear()
        activateLens(lensId)
    }

    property variant lenses: Lenses {}

    /* If unity-2d-places is launched on demand by an activateLens() dbus call,
       "lenses" is not yet populated, so activating "commands.lens",
       for example, triggered by Alt+F2 fails.
       This following connection fixes this issue by checking if any lenses
       should be activated as long as "lenses" is being populated. lp:883392 */
    Connections {
        target: lenses
        onRowsInserted: {
            if (queuedLensId != "") {
                var lens = lenses.get(queuedLensId)
                if (lens != null) {
                    activateLens(queuedLensId)
                    queuedLensId = "";
                    return
                }
            }
        }
    }

    Item {
        id: background

        anchors.fill: parent

        /* Avoid redraw at rendering */
        effect: CacheEffect {}

        Item {
            anchors.fill: parent
            anchors.bottomMargin: content.anchors.bottomMargin
            anchors.rightMargin: content.anchors.rightMargin
            clip: true

            Image {
                id: blurredBackground

                effect: Blur {blurRadius: 12}

                /* 'source' needs to be set when the dash becomes visible, that
                   is when declarativeView.active becomes true, so that a
                   screenshot of the windows behind the dash is taken at that
                   point.
                   See http://doc.qt.nokia.com/4.7-snapshot/qml-image.html#cache-prop
                */

                /* Use an image of the root window which essentially is a
                   capture of the entire screen */
                source: declarativeView.active ? "image://window/root" : ""
                cache: false

                fillMode: Image.PreserveAspectCrop
                x: -declarativeView.globalPosition.x
                y: -declarativeView.globalPosition.y
            }

            Image {
                anchors.fill: parent
                fillMode: Image.PreserveAspectCrop
                source: "artwork/background_sheen.png"
            }
        }

        BorderImage {
            anchors.fill: parent
            visible: dashView.dashMode == DashDeclarativeView.DesktopMode
            source: desktop.isCompositingManagerRunning ? "artwork/desktop_dash_background.sci" : "artwork/desktop_dash_background_no_transparency.sci"
            mirror: isRightToLeft()
        }
    }

    Item {
        id: content

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
        /* FIXME: deactivated because it makes the user lose the focus very often */
        //Keys.forwardTo: [search_entry]


        SearchEntry {
            id: search_entry

            focus: true
            /* FIXME: check on visible necessary; fixed in Qt Quick 1.1
                      ref: http://bugreports.qt.nokia.com/browse/QTBUG-15862
            */
            KeyNavigation.right: filterPane.visible ? filterPane : search_entry
            KeyNavigation.down: pageLoader

            anchors.top: parent.top
            anchors.topMargin: 11
            anchors.left: parent.left
            anchors.leftMargin: 10
            anchors.right: filterPane.left
            anchors.rightMargin: 15

            height: 42
        }

        FilterPane {
            id: filterPane

            KeyNavigation.left: search_entry

            /* FilterPane is only to be displayed for lenses, not in the home page or Alt+F2 Run page */
            visible: dashView.activeLens != "" && dashView.activeLens != "commands.lens"
            lens: visible && currentPage != undefined ? currentPage.model : undefined

            anchors.top: search_entry.anchors.top
            anchors.topMargin: search_entry.anchors.topMargin
            anchors.bottom: lensBar.top
            headerHeight: search_entry.height
            width: 300
            anchors.right: parent.right
            anchors.rightMargin: 15
        }

        Loader {
            id: pageLoader
            objectName: "pageLoader"

            Accessible.name: "loader"
            /* FIXME: check on visible necessary; fixed in Qt Quick 1.1
                      ref: http://bugreports.qt.nokia.com/browse/QTBUG-15862
            */
            KeyNavigation.right: filterPane.visible && !filterPane.folded ? filterPane : pageLoader
            KeyNavigation.up: search_entry
            KeyNavigation.down: lensBar

            anchors.top: search_entry.bottom
            anchors.topMargin: 9
            anchors.bottom: lensBar.top
            anchors.left: parent.left
            anchors.right: !filterPane.visible || filterPane.folded ? parent.right : filterPane.left
            anchors.rightMargin: !filterPane.visible || filterPane.folded ? 0 : 15
            onLoaded: item.focus = true
        }

        LensBar {
            id: lensBar

            KeyNavigation.up: pageLoader

            anchors.bottom: parent.bottom
            anchors.left: parent.left
            anchors.right: parent.right
            height: 44
            visible: dashView.expanded
        }
    }
}
