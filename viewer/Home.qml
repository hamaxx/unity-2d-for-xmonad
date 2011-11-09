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
import Unity2d 1.0

FocusScope {
    property variant model: PageModel {
        /* model.searchQuery is copied over to all lenses globalSearchQuery property */
        onSearchQueryChanged: {
            for (var i = 0; i < dash.lenses.rowCount(); i++) {
                dash.lenses.get(i).globalSearchQuery = searchQuery
            }
        }
    }

    function activateFirstResult() {
        /* Going through the list of lenses and selecting the first one
           that has results for the global search, that is items in its
           globalResults */
        var lens, i
        for (i=0; i<dash.lenses.rowCount(); i=i+1) {
            lens = dash.lenses.get(i)
            if (lens.globalResults != null && lens.globalResults.count != 0) {
                var firstResult = lens.globalResults.get(0)
                /* Lenses give back the uri of the item in 'column_0' per specification */
                var uri = firstResult.column_0
                dashView.active = false
                lens.activate(decodeURIComponent(uri))
                return;
            }
        }
    }

    /* Set to false to hide the shortcuts buttons */
    property bool shortcutsActive: true

    /* Either globalSearch is shown or buttons are shown depending on globalSearchActive */
    property bool globalSearchActive: model.searchQuery != ""
    
    /* Used by dash.qml to bind to dashView "expanded" property */
    property bool expanded: globalSearchActive || shortcutsActive

    AbstractButton {
        id: openShortcutsButton

        Accessible.name: "Open Shortcuts"

        anchors.bottom: parent.top
        anchors.right: parent.right
        anchors.rightMargin: 50
        anchors.bottomMargin: 10
        width: childrenRect.width
        height: childrenRect.height

        Image {
            id: icon
            source: "artwork/open_shortcuts.png"
            width: sourceSize.width
            height: sourceSize.height
            anchors.left: parent.left
        }

        TextCustom {
            text: u2d.tr("Shortcuts")
            anchors.left: icon.right
            anchors.leftMargin: 3
            width: paintedWidth
            height: icon.height
            fontSize: "large"
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
        }

        opacity: (!expanded && dashView.dashMode == DashDeclarativeView.DesktopMode) ? 1 : 0
        Behavior on opacity {NumberAnimation {duration: 100}}

        onClicked: {
            shortcutsActive = true
        }
    }

    ListViewWithScrollbar {
        id: globalSearch

        focus: globalSearchActive
        opacity: globalSearchActive ? 1 : 0
        anchors.fill: parent
        anchors.leftMargin: 20

        model: dash.lenses

        bodyDelegate: TileVertical {
            lens: model.item
            name: model.item.name
            iconHint: model.item.iconHint

            category_model: model.item.globalResults
            property bool focusable: category_model != undefined && category_model.count > 0
        }

        headerDelegate: CategoryHeader {
            visible: body.needHeader && body.focusable
            height: visible ? 32 : 0

            property bool foldable: body.folded != undefined
            availableCount: foldable && body.category_model != null ? body.category_model.count - body.cellsPerRow : 0
            folded: foldable ? body.folded : false
            onClicked: if(foldable) body.folded = !body.folded

            icon: body.iconHint
            label: body.name
        }
    }

    FocusScope {
        id: shortcuts

        focus: !globalSearchActive
        opacity: (!globalSearchActive && (shortcutsActive || dashView.dashMode == DashDeclarativeView.FullScreenMode)) ? 1 : 0
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.verticalCenter: parent.verticalCenter

        width: 888
        height: 436

        Rectangle {
            anchors.fill: parent
            radius: 5
            border.width: 1
            /* FIXME: wrong colors */
            border.color: Qt.rgba(1, 1, 1, 0.2)
            color: Qt.rgba(0, 0, 0, 0.3)
        }

        AbstractButton {
            id: closeShortcutsButton

            anchors.left: parent.left
            anchors.top: parent.top
            anchors.leftMargin: -width/2

            width: childrenRect.width
            height: childrenRect.height

            Image {
                id: search_icon

                width: sourceSize.width
                height: sourceSize.height

                source: "artwork/cross.png"
            }

            opacity: (expanded && dashView.dashMode == DashDeclarativeView.DesktopMode) ? 1 : 0
            Behavior on opacity {NumberAnimation {duration: 100}}

            onClicked: shortcutsActive = false
        }

        /* Try to load a custom version of the shortcuts first, and fall back
           on the default version if a custom one doesn’t exist. */
        Loader {
            id: customShortcutsLoader
            focus: status == Loader.Ready
            anchors.fill: parent
            source: "HomeShortcutsCustomized.qml"
        }
        Loader {
            id: defaultShortcutsLoader
            focus: !customShortcutsLoader.focus
            anchors.fill: parent
            source: (customShortcutsLoader.status == Loader.Error) ? "HomeShortcuts.qml" : ""
        }
    }
}
