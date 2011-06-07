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

import Qt 4.7
import Unity2d 1.0 /* Necessary for SortFilterProxyModel and for the ImageProvider serving image://icons/theme_name/icon_name */
import Places 1.0 /* Necessary for DashDeclarativeView.*Dash */

Item {
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
            if (placeEntry.globalResultsModel != null && placeEntry.globalResultsModel.count() != 0) {
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

        opacity: globalSearchActive ? 1 : 0
        anchors.fill: parent

        list.model: dash.places

        list.delegate: UnityDefaultRenderer {
            width: ListView.view.width

            parentListView: list
            placeEntryModel: item
            displayName: item.name
            iconHint: item.icon

            /* Filter out results for which the corresponding group's renderer
               is 'UnityEmptySearchRenderer'.
               Each result has a column (the second one) containing the id of
               the group it belongs to (groupId).
            */
            model:  SortFilterProxyModel {
                model: item.globalResultsModel

                /* FIXME: we ignore the groupId with renderer 'UnityEmptySearchRenderer'
                   by hardcoding it instead of looking it up in the Place's
                   groupsModel as Unity does.

                   Two solutions could be envisioned:
                   1) Actually looking for the row in the Place's groupsModel
                      that has in its first column 'UnityEmptySearchRenderer'.
                      That would require adding an API in libqtdee's DeeListModel.
                   2) Changing the behaviour of the place daemons so that the
                      Place's globalResultsModel is empty when there are no
                      results. The applications place does that but not the
                      files place.
                */
                property int ignoredGroupId: 5
                filterRole: 2 /* groupId column */
                filterRegExp: RegExp("^[^%1]$".arg(ignoredGroupId)) /* anything but the ignoredGroupId */
            }
        }
    }

    Rectangle {
        id: shortcuts

        opacity: (!globalSearchActive && (shortcutsActive || dashView.dashMode == DashDeclarativeView.FullScreenMode)) ? 1 : 0
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.verticalCenter: parent.verticalCenter

        width: 888
        height: 466

        radius: 5
        border.width: 1
        /* FIXME: wrong colors */
        border.color: Qt.rgba(1, 1, 1, 0.2)
        color: Qt.rgba(0, 0, 0, 0.3)

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
            anchors.fill: parent
            source: "HomeShortcutsCustomized.qml"
        }
        Loader {
            id: defaultShortcutsLoader
            anchors.fill: parent
            source: (customShortcutsLoader.status == Loader.Error) ? "HomeShortcuts.qml" : ""
        }
    }
}
