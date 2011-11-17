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
import "utils.js" as Utils

FocusScope {
    id: lensView

    /* An instance of Lens */
    property variant model

    function activateFirstResult() {
        /* Going through the list of categories and selecting the first one
           that has results for the search. A SortFilterProxyModel
           ('firstCategoryModel') is used to filter the search results per category.
        */
        var i
        for (i=0; i<lensView.model.categories.count; i=i+1) {
            firstCategoryModel.categoryId = i
            if (firstCategoryModel.count != 0) {
                var firstResult = firstCategoryModel.get(0)
                /* Lenses give back the uri of the item in 'column_0' per specification */
                var uri = firstResult.column_0
                dashView.active = false
                model.activate(decodeURIComponent(uri))
                return;
            }
        }
    }

    SortFilterProxyModel {
        id: firstCategoryModel
        property int categoryId
        model: lensView.model != undefined ? lensView.model.results : null

        /* lensView.model.entryResultsModel contains data for all
           the categories of a given Lens.
           Each row has a column (the second one) containing the id of
           the category it belongs to (categoryId).
        */
        filterRole: 2 /* second column (see above comment) */
        filterRegExp: RegExp("^%1$".arg(categoryId)) /* exact match */
    }

    ListViewWithScrollbar {
        id: results
        focus: true
        anchors.fill: parent
        anchors.leftMargin: 20

        /* The category's delegate is chosen dynamically depending on what
           rendererName is returned by the CategoriesModel.

           Each rendererName should have a corresponding QML file with the
           same name that will be used as delegate.
           For example:

           If rendererName == 'UnityShowcaseRenderer' then it will look for
           the file 'UnityShowcaseRenderer.qml' and use it to render the category.
        */
        bodyDelegate: Loader {
            visible: category_model.count > 0
            focus: true
            width: parent.width
            height: item.contentHeight
            FocusPath.skip: false

            property string name: model.column_0
            property string iconHint: model.column_1
            property string rendererName: model.column_2
            property int categoryId: index

            source: rendererName ? Utils.convertToCamelCase(rendererName) + ".qml" : ""
            onStatusChanged: {
                if (status == Loader.Error) {
                    console.log("Failed to load renderer %1. Using default renderer instead.".arg(rendererName))
                    source = "TileVertical.qml"
                } else {
                    item.focus = true
                }
            }

            /* Model that will be used by the category's delegate */
            property variant category_model: SortFilterProxyModel {
                model: lensView.model.results

                /* resultsModel contains data for all the categories of a given Lens.
                   Each row has a column (the second one) containing the id of
                   the category it belongs to (categoryId).
                */
                filterRole: 2 /* second column (see above comment) */
                filterRegExp: RegExp("^%1$".arg(categoryId)) /* exact match */
            }

            /* Required by ListViewWithHeaders when the loaded Renderer is a Flickable.
               In that case the list view scrolls the Flickable appropriately.
            */
            property int totalHeight: item.totalHeight != undefined ? item.totalHeight : 0
            property int cellsPerRow: item.cellsPerRow
            property variant currentItem: item.currentItem

            /*This is necessary because the alias does not work in a loaded item.
              Is not possible create alias for this property because that property does not exist
              during the object creation, this property will became available after the load item process .
             */
            property bool folded: item.folded
            onFoldedChanged: item.folded = folded

            Binding { target: item; property: "name"; value: name }
            Binding { target: item; property: "iconHint"; value: iconHint }
            Binding { target: item; property: "categoryId"; value: categoryId }
            Binding { target: item; property: "category_model"; value: category_model }
            Binding { target: item; property: "lens"; value: lensView.model }
        }

        headerDelegate: CategoryHeader {
            visible: body.item.needHeader && body.visible
            height: visible ? 32 : 0
            availableCount: foldable && body.category_model != null ? body.category_model.count - body.cellsPerRow : 0
            folded: foldable ? body.folded : false
            focus: true
            icon: body.iconHint
            label: body.name

            property bool foldable: body.folded != undefined

            onClicked: if(foldable) body.folded = !body.folded
            moving: flickerMoving
        }

        model: lensView.model != undefined ? lensView.model.categories : undefined
    }
}
