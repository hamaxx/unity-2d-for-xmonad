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

FocusScope {
    id: dash
    objectName: "Dash"
    Accessible.name: "dash"

    LayoutMirroring.enabled: Utils.isRightToLeft()
    LayoutMirroring.childrenInherit: true

    property variant currentPage
    /* FIXME: 'active' property exactly mirrors 'shellManager.dashActive'.
       The final goal is to transition to using exclusively the QML 'active' property
       and drop the C++ 'shellManager.dashActive'.
    */
    property variant active
    /* The following way of mirroring the values of 'shellManager.dashActive'
       and 'active' works now and QML does not see it as a binding loop but we
       cannot count on it long term.
    */
    Binding {
        target: shellManager
        property: "dashActive"
        value: dash.active
    }
    Binding {
        target: dash
        property: "active"
        value: shellManager.dashActive
    }

    onActiveChanged: if (dash.active) shellManager.dashShell.forceActivateWindow()

    Connections {
        target: shellManager
        onDashShellChanged: {
            if (dash.active) {
                background.trigger()
                shellManager.dashShell.forceActivateWindow()
            }
        }
    }

    property variant queuedLensId

    property bool expanded: (currentPage && currentPage.expanded != undefined) ? currentPage.expanded : true

    Binding {
        target: shellManager
        property: "dashMode"
        value: shellManager.dashAlwaysFullScreen || dash2dConfiguration.fullScreen ?
               ShellManager.FullScreenMode : ShellManager.DesktopMode
    }

    Connections {
        target: shellManager

        onDashActivateHome: activateHome()
        onDashActivateLens: activateLens(lensId)
    }

    Connections {
        target: shellManager.dashShell

        onFocusChanged: if (!shellManager.dashShell.focus) active = false
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
        pageLoader.setSource("LensView.qml")
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
        shellManager.dashActiveLens = ""
        pageLoader.setSource("")
    }

    SpreadMonitor {
        id: spreadMonitor
    }

    function activateLens(lensId) {
        if (spreadMonitor.shown) return

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

        if (lensId == shellManager.dashActiveLens && dash.active) {
            /* we don't need to activate the lens, just show its UI */
            buildLensPage(lens)
            return
        }

        for (var i=0; i<lenses.rowCount(); i++) {
            var thislens = lenses.get(i)
            if (lensId == "home.lens") {
                if (thislens.id == lensId) {
                    thislens.viewType = Lens.LensView
                    continue
                }
                /* When Home is shown, need to notify all other lenses. Those in the global view
                    (in home search results page) are set to HomeView, all others to Hidden */
                thislens.viewType = (thislens.searchInGlobal) ? Lens.HomeView : Lens.Hidden
            } else {
                thislens.viewType = (lens == thislens) ?  Lens.LensView : Lens.Hidden
            }
        }

        buildLensPage(lens)
        shellManager.dashActiveLens = lens.id
        dash.active = true
    }

    function activateHome() {
        if (spreadMonitor.shown) return
        if (shellManager.dashHaveCustomHomeShortcuts) {
            for (var i=0; i<lenses.rowCount(); i++) {
                lenses.get(i).viewType = Lens.Hidden
            }
            pageLoader.setSource("Home.qml")
            /* Take advantage of the fact that the loaded qml is local and setting
               the source loads it immediately making pageLoader.item valid */
            activatePage(pageLoader.item)
            shellManager.dashActiveLens = ""
            dash.active = true
        } else {
            activateLens("home.lens")
        }
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

    function activateUriWithLens(lens, uri, mimetype) {
        dash.active = false
        lens.activate(decodeURIComponent(uri))
    }

    function activateApplication(application) {
        dash.active = false
        application.activate()
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

    Background {
        id: background

        anchors.fill: parent

        active: dash.active
        fullscreen: shellManager.dashMode != ShellManager.DesktopMode
        view: shellManager.dashShell
    }

    Item {
        id: content

        anchors.fill: parent
        /* Margins in DesktopMode set so that the content does not overlap with
           the border defined by the background image.
        */
        anchors.bottomMargin: background.bottomBorderThickness
        anchors.rightMargin: background.rightBorderThickness

        /* Unhandled keys will always be forwarded to the search bar. That way
           the user can type and search from anywhere in the interface without
           necessarily focusing the search bar first. */
        /* FIXME: deactivated because it makes the user lose the focus very often */
        //Keys.forwardTo: [search_entry]

        Image {
            id: panelBorder

            height: 1
            anchors.top: parent.top
            anchors.left: parent.left
            anchors.right: parent.right
            source: "../common/artwork/panel_border.png"
            fillMode: Image.Stretch
        }

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

            active: dash.active
            placeHolderText: {
                if(dash.currentPage != undefined && dash.currentPage.model.searchHint)
                    return dash.currentPage.model.searchHint
                else
                    return u2d.tr("Search")
            }

            onActivateFirstResult: if (dash.currentPage != undefined) dash.currentPage.activateFirstResult()
        }

        Binding {
            /* not using 'when' clause since target needs these check anyway, otherwise it gives warnings if dash.currentPage is undefined */
            target: dash.currentPage != undefined ? dash.currentPage.model : null
            property: "searchQuery"
            value: search_entry.searchQuery
        }

        FilterPane {
            id: filterPane

            KeyNavigation.left: search_entry

            /* FilterPane is only to be displayed for lenses, not in the home page or Alt+F2 Run page */
            visible: shellManager.dashActiveLens != "home.lens" && shellManager.dashActiveLens != "" && shellManager.dashActiveLens != "commands.lens"
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
            anchors.bottom: lensBar.top
            anchors.left: parent.left
            anchors.right: !filterPane.visible || filterPane.folded ? parent.right : filterPane.left
            anchors.rightMargin: !filterPane.visible || filterPane.folded ? 0 : 15
            onLoaded: item.focus = true

            /* Workaround loss of focus issue happening when the loaded item has
               active focus and is then destroyed. The active focus was completely
               lost instead of being relinquished to the Loader.

               Ref.: https://bugreports.qt.nokia.com/browse/QTBUG-22939
            */
            function setSource(newSource) {
                var hadActiveFocus = activeFocus
                source = newSource
                if (hadActiveFocus) forceActiveFocus()
            }
        }

        LensBar {
            id: lensBar

            KeyNavigation.up: pageLoader

            anchors.bottom: parent.bottom
            anchors.left: parent.left
            anchors.right: parent.right
            height: 44
            visible: expanded
        }
    }

    property int desktopCollapsedHeight: 115
    property int desktopExpandedHeight: 615
    property int desktopWidth: 996

    states: [
        State {
            name: "desktop"
            when: shellManager.dashMode == ShellManager.DesktopMode
            PropertyChanges {
                target: dash
                width: desktopWidth
                height: expanded ? desktopExpandedHeight : desktopCollapsedHeight
            }
        },
        State {
            name: "fullscreen"
            when: shellManager.dashMode == ShellManager.FullScreenMode
            PropertyChanges {
                target: dash
                width: shellManager.dashShell != undefined ? shellManager.dashShell.screen.panelsFreeGeometry.width : 0
                height: shellManager.dashShell != undefined ? shellManager.dashShell.screen.panelsFreeGeometry.height : 0
            }
        }
    ]
}
