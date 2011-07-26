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
import Unity2d 1.0 /* Necessary for SortFilterProxyModel */

FocusScope {
    id: placeEntryView

    /* An instance of PlaceEntryModel */
    property variant model

    function activateFirstResult() {
        /* Going through the list of groups and selecting the first one
           that has results for the search. A SortFilterProxyModel
           ('firstGroupModel') is used to filter the search results per group.
        */
        var placeEntry, i
        for (i=0; i<placeEntryView.model.entryGroupsModel.count; i=i+1) {
            firstGroupModel.groupId = i
            if (firstGroupModel.count != 0) {
                var firstResult = firstGroupModel.get(0)
                /* Places give back the uri of the item in 'column_0' per specification */
                var uri = firstResult.column_0
                dashView.active = false
                model.place.activate(decodeURIComponent(uri))
                return;
            }
        }
    }

    SortFilterProxyModel {
        id: firstGroupModel

        property int groupId
        model: placeEntryView.model.entryResultsModel

        /* placeEntryView.model.entryResultsModel contains data for all
           the groups of a given Place.
           Each row has a column (the second one) containing the id of
           the group it belongs to (groupId).
        */
        filterRole: 2 /* second column (see above comment) */
        filterRegExp: RegExp("^%1$".arg(groupId)) /* exact match */
    }

    ListViewWithScrollbar {
        id: results

        focus: true
        anchors.fill: parent

        /* The group's delegate is chosen dynamically depending on what
           groupRenderer is returned by the GroupsModel.

           Each groupRenderer should have a corresponding QML file with the
           same name that will be used as delegate.
           For example:

           If groupRenderer == 'UnityShowcaseRenderer' then it will look for
           the file 'UnityShowcaseRenderer.qml' and use it to render the group.
        */
        bodyDelegate: Loader {
            property string groupRenderer: model.column_0
            property string displayName: model.column_1
            property string iconHint: model.column_2
            property int groupId: index

            source: groupRenderer ? groupRenderer+".qml" : ""
            onStatusChanged: {
                if (status == Loader.Error)
                    console.log("Failed to load renderer", groupRenderer)
            }

            /* Model that will be used by the group's delegate */
            property variant group_model: SortFilterProxyModel {
                model: placeEntryView.model.entryResultsModel

                /* resultsModel contains data for all the groups of a given Place.
                   Each row has a column (the second one) containing the id of
                   the group it belongs to (groupId).
                */
                filterRole: 2 /* second column (see above comment) */
                filterRegExp: RegExp("^%1$".arg(groupId)) /* exact match */
            }

            /* Required by ListViewWithHeaders when the loaded Renderer is a Flickable.
               In that case the list view scrolls the Flickable appropriately.
            */
            property int totalHeight: item.totalHeight != undefined ? item.totalHeight : 0
            property int contentY
            Binding { target: item; property: "contentY"; value: contentY }
            property bool focusable: group_model.count > 0

            onLoaded: {
                item.displayName = displayName
                item.iconHint = iconHint
                item.groupId = groupId
                item.group_model = group_model
                item.placeEntryModel = placeEntryView.model
                item.focus = true
            }
        }

        headerDelegate: GroupHeader {
            visible: body.item.needHeader && body.focusable
            height: visible ? 32 : 0

            property bool foldable: body.item.folded != undefined
            availableCount: foldable ? body.group_model.count - body.item.cellsPerRow : 0
            folded: foldable ? body.item.folded : false
            onClicked: if(foldable) body.item.folded = !body.item.folded

            icon: body.iconHint
            label: body.displayName
        }

        model: placeEntryView.model != undefined ? placeEntryView.model.entryGroupsModel : undefined
    }
}
