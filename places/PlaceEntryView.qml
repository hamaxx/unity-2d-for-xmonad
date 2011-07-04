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

Item {
    id: placeEntryView

    /* An instance of PlaceEntryModel */
    property variant model

    function activateFirstResult() {
        /* Going through the list of groups and selecting the first one
           that has results for the search. A SortFilterProxyModel
           ('firstGroupModel') is used to filter the search results per group.
        */
        var placeEntry, i
        for (i=0; i<placeEntryView.model.entryGroupsModel.count(); i=i+1) {
            firstGroupModel.groupId = i
            if (firstGroupModel.count() != 0) {
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

        anchors.fill: parent

        /* The group's delegate is chosen dynamically depending on what
           groupRenderer is returned by the GroupsModel.

           Each groupRenderer should have a corresponding QML file with the
           same name that will be used as delegate.
           For example:

           If groupRenderer == 'UnityShowcaseRenderer' then it will look for
           the file 'UnityShowcaseRenderer.qml' and use it to render the group.
        */
        list.delegate: Loader {
            property string groupRenderer: column_0
            property string displayName: column_1
            property string iconHint: column_2
            property int groupId: index

            source: groupRenderer ? groupRenderer+".qml" : ""
            onStatusChanged: {
                if (status == Loader.Error)
                    console.log("Failed to load renderer", groupRenderer)
            }

            width: ListView.view.width

            /* Model that will be used by the group's delegate */
            SortFilterProxyModel {
                id: group_model

                model: placeEntryView.model.entryResultsModel

                /* resultsModel contains data for all the groups of a given Place.
                   Each row has a column (the second one) containing the id of
                   the group it belongs to (groupId).
                */
                filterRole: 2 /* second column (see above comment) */
                filterRegExp: RegExp("^%1$".arg(groupId)) /* exact match */
            }

            onLoaded: {
                item.parentListView = results.list
                item.displayName = displayName
                item.iconHint = iconHint
                item.groupId = groupId
                item.model = group_model
                item.placeEntryModel = placeEntryView.model
            }
        }

        list.model: placeEntryView.model != undefined ? placeEntryView.model.entryGroupsModel : undefined
    }
}
