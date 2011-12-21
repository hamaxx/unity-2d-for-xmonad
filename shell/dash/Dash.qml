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
import "../common"

FocusScope {
    id: dash
    Accessible.name: "dash"

    LayoutMirroring.enabled: isRightToLeft()
    LayoutMirroring.childrenInherit: true

    property variant currentPage
    /* FIXME: 'active' property exactly mirrors 'declarativeView.dashActive'.
       The final goal is to transition to using exclusively the QML 'active' property
       and drop the C++ 'declarativeView.dashActive'.
    */
    property variant active
    /* The following way of mirroring the values of 'declarativeView.dashActive'
       and 'active' works now and QML does not see it as a binding loop but we
       cannot count on it long term.
    */
    Binding {
        target: declarativeView
        property: "dashActive"
        value: dash.active
    }
    Binding {
        target: dash
        property: "active"
        value: declarativeView.dashActive
    }

    /* Unload the current page when closing the dash */
    onActiveChanged: {
        if (!dash.active) {
            /* FIXME: currentPage needs to stop pointing to pageLoader.item
                      that is about to be invalidated otherwise a crash
                      occurs because SearchEntry has a binding that refers
                      to currentPage and tries to access it.
               Ref.: https://bugs.launchpad.net/ubuntu/+source/unity-2d/+bug/817896
                     https://bugreports.qt.nokia.com/browse/QTBUG-20692
            */
            deactivateActiveLens()
            currentPage = undefined
            // Delay the following instruction by 1 millisecond using a
            // timer. This is enough to work around a crash that happens
            // when the layout is mirrored (RTL locales). See QTBUG-22776
            // for details.
            //pageLoader.source = ""
            delayPageLoaderReset.restart()
        }
    }

    Timer {
        id: delayPageLoaderReset
        interval: 1
        onTriggered: pageLoader.setSource("")
    }

    function isRightToLeft() {
        return Qt.application.layoutDirection == Qt.RightToLeft
    }

    Binding {
        target: declarativeView
        property: "expanded"
        value: (currentPage && currentPage.expanded != undefined) ? currentPage.expanded : true
    }

    DashSettings {
        id: dashSettings
    }

    Binding {
        target: declarativeView
        property: "dashMode"
        value: (dashSettings.formFactor == DashSettings.Desktop) ?
               ShellDeclarativeView.DesktopMode : ShellDeclarativeView.FullScreenMode
    }

    Connections {
        target: declarativeView

        onActivateHome: activateHome()
        onActivateLens: activateLens(lensId)
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

    function deactivateActiveLens() {
        if (declarativeView.activeLens != "") {
            var lens = lenses.get(declarativeView.activeLens)
            lens.active = false
        }
    }

    function buildLensPage(lens) {
        pageLoader.setSource("LensView.qml")
        /* Take advantage of the fact that the loaded qml is local and setting
           the source loads it immediately making pageLoader.item valid */
        pageLoader.item.model = lens
        activatePage(pageLoader.item)
    }

    function activateLens(lensId) {
        var lens = lenses.get(lensId)
        if (lens == null) {
            console.log("No match for lens: %1".arg(lensId))
            return
        }

        if (lensId == declarativeView.activeLens) {
            /* we don't need to activate the lens, just show its UI */
            buildLensPage(lens)
            return
        }

        deactivateActiveLens()
        lens.active = true
        buildLensPage(lens)
        declarativeView.activeLens = lens.id
        dash.active = true
    }

    function activateHome() {
        deactivateActiveLens()
        pageLoader.setSource("Home.qml")
        /* Take advantage of the fact that the loaded qml is local and setting
           the source loads it immediately making pageLoader.item valid */
        activatePage(pageLoader.item)
        declarativeView.activeLens = ""
        dash.active = true
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
                   is when dash.active becomes true, so that a
                   screenshot of the windows behind the dash is taken at that
                   point.
                   'source' also needs to change so that the screenshot is
                   re-taken as opposed to pulled from QML's image cache.
                   This workarounds the fact that the image cache cannot be
                   disabled. A new API to deal with this was introduced in Qt Quick 1.1.

                   See http://doc.qt.nokia.com/4.7-snapshot/qml-image.html#cache-prop
                */
                property variant timeAtActivation
                Connections {
                    target: dash
                    onActiveChanged: blurredBackground.timeAtActivation = screen.currentTime()
                }

                /* Use an image of the root window which essentially is a
                   capture of the entire screen */
                source: dash.active ? "image://window/root@" + blurredBackground.timeAtActivation : ""

                fillMode: Image.PreserveAspectCrop
                x: -launcherLoader.width
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
            visible: declarativeView.dashMode == ShellDeclarativeView.DesktopMode
            source: screen.isCompositingManagerRunning ? "artwork/desktop_dash_background.sci" : "artwork/desktop_dash_background_no_transparency.sci"
            mirror: isRightToLeft()
        }
    }

    Item {
        id: content

        anchors.fill: parent
        /* Margins in DesktopMode set so that the content does not overlap with
           the border defined by the background image.
        */
        anchors.bottomMargin: declarativeView.dashMode == ShellDeclarativeView.DesktopMode ? 39 : 0
        anchors.rightMargin: declarativeView.dashMode == ShellDeclarativeView.DesktopMode ? 37 : 0

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
            visible: declarativeView.activeLens != "" && declarativeView.activeLens != "commands.lens"
            lens: visible && currentPage != undefined ? currentPage.model : undefined

            anchors.top: search_entry.anchors.top
            anchors.topMargin: search_entry.anchors.topMargin
            anchors.bottom: lensBar.top
            headerHeight: search_entry.height
            width: 300
            anchors.right: parent.right
            anchors.rightMargin: 8
        }

        Loader {
            id: pageLoader

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
            visible: declarativeView.expanded
        }
    }

    AbstractButton {
        id: fullScreenButton

        Accessible.name: "Full Screen"

        anchors.bottom: parent.bottom
        anchors.right: parent.right
        anchors.rightMargin: 15
        anchors.bottomMargin: 15
        width: fullScreenButtonImage.sourceSize.width
        height: fullScreenButtonImage.sourceSize.height
        visible: declarativeView.dashMode != ShellDeclarativeView.FullScreenMode

        Image {
            id: fullScreenButtonImage
            source: "artwork/fullscreen_button.png"
            mirror: isRightToLeft()
        }

        onClicked: {
            declarativeView.dashMode = ShellDeclarativeView.FullScreenMode
        }
    }

    property int desktopCollapsedHeight: 115
    property int desktopExpandedHeight: 606
    property int desktopWidth: 989
    property int fullscreenWidth: screen.availableGeometry.width
    property int fullscreenHeight: screen.availableGeometry.height

    states: [
        State {
            name: "desktop"
            when: declarativeView.dashMode == ShellDeclarativeView.DesktopMode
            PropertyChanges {
                target: dash
                width: desktopWidth
                height: declarativeView.expanded ? desktopExpandedHeight : desktopCollapsedHeight
            }
        },
        State {
            name: "fullscreen"
            when: declarativeView.dashMode == ShellDeclarativeView.FullScreenMode
            PropertyChanges {
                target: dash
                width: fullscreenWidth
                height: fullscreenHeight
            }
        }
    ]
}
