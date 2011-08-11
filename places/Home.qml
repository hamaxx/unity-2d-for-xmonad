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
        /* model.entrySearchQuery is copied over to all place entries's globalSearchQuery property */
        onEntrySearchQueryChanged: {
            for (var i = 0; i < dash.places.rowCount(); i++) {
                dash.places.get(i).globalSearchQuery = entrySearchQuery
            }
        }
    }

    function activateFirstResult() {
        /* Going through the list of place entries and selecting the first one
           that has results for the global search, that is items in its
           globalResultsModel */
        var placeEntry, i
        for (i=0; i<dash.places.rowCount(); i=i+1) {
            placeEntry = dash.places.get(i)
            if (placeEntry.globalResultsModel != null && placeEntry.globalResultsModel.count != 0) {
                var firstResult = placeEntry.globalResultsModel.get(0)
                /* Places give back the uri of the item in 'column_0' per specification */
                var uri = firstResult.column_0
                dashView.active = false
                placeEntry.place.activate(decodeURIComponent(uri))
                return;
            }
        }
    }

    /* Set to false to hide the shortcuts buttons */
    property bool shortcutsActive: true

    /* Either globalSearch is shown or buttons are shown depending on globalSearchActive */
    property bool globalSearchActive: model.entrySearchQuery != ""
    
    /* Used by dash.qml to bind to dashView "expanded" property */
    property bool expanded: globalSearchActive || shortcutsActive

    Button {
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
            font.pixelSize: 16
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

        model: dash.places

        bodyDelegate: UnityDefaultRenderer {
            placeEntryModel: model.item
            displayName: model.item.name
            iconHint: model.item.icon

            group_model: model.item.globalResultsModel
            property bool focusable: group_model != undefined && group_model.count > 0
        }

        headerDelegate: GroupHeader {
            visible: body.needHeader && body.focusable
            height: visible ? 32 : 0

            property bool foldable: body.folded != undefined
            availableCount: foldable && body.group_model != null ? body.group_model.count - body.cellsPerRow : 0
            folded: foldable ? body.folded : false
            onClicked: if(foldable) body.folded = !body.folded

            icon: body.iconHint
            label: body.displayName
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

        Button {
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
