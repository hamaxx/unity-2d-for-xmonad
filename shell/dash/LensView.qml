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
import "../common/utils.js" as Utils

FocusScope {
    id: lensView

    /* An instance of Lens */
    property variant model
    property string firstNonEmptyCategory

    function scrollToTop() {
        results.scrollbar.targetFlickable.contentY = 0
    }

    function updateFirstCategory() {
        if (lensView.model.results.count == 0)
            return
        var firstCategory = -1
        for (var i = 0; i < lensView.model.results.count; i++) {
            var result = lensView.model.results.get(i)
            if (result.column_2 < firstCategory || firstCategory == -1) {
                firstCategory = result.column_2
                if (firstCategory == 0) {
                    break
                }
            }
        }
        var category = lensView.model.categories.get(firstCategory)
        firstNonEmptyCategory = category.column_0
    }

    Connections {
        target: lensView.model.results
        onCountChanged: updateFirstCategory()
    }

    function activateFirstResult() {
        var firstResult = null
        for (var i = 0; i < lensView.model.results.count; i++) {
            var result = lensView.model.results.get(i)
            var category = result.column_2
            if ((firstResult === null) || (category < firstResult.column_2)) {
                firstResult = result
            }
        }
        if (firstResult === null) {
            return
        }
        var uri = firstResult.column_0
        var mimetype = firstResult.column_3
        dash.activateUriWithLens(model, uri, mimetype)
    }

    ListViewWithScrollbar {
        id: results

        focus: true
        anchors.fill: parent
        anchors.leftMargin: 15

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
            width: parent.width
            height: item ? visible ? item.contentHeight : 0 : 0

            property string name: model.column_0
            property string iconHint: model.column_1
            property string rendererName: model.column_2
            property int categoryId: index

            source: rendererName ? Utils.convertToCamelCase(rendererName) + ".qml" : ""
            onStatusChanged: {
                updateFirstCategory()
                if (status == Loader.Error) {
                    console.log("Failed to load renderer %1. Using default renderer instead.".arg(rendererName))
                    source = "TileVertical.qml"
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
            property variant currentItem: item.currentItem

            Binding { target: item; property: "name"; value: name }
            Binding { target: item; property: "iconHint"; value: iconHint }
            Binding { target: item; property: "categoryId"; value: categoryId }
            Binding { target: item; property: "category_model"; value: category_model }
            Binding { target: item; property: "lens"; value: lensView.model }
            Binding { target: item; property: "lensId"; value: lensView.model.id }

            onLoaded: item.focus = true
        }

        headerDelegate: CategoryHeader {
            visible: body.item ? body.item.needHeader && body.visible : false
            height: visible ? 35 : 0

            property bool isFirst: firstNonEmptyCategory == body.name
            property bool foldable: body.item ? body.item.folded != undefined : false
            availableCount: body.item ? foldable ? body.category_model.count - body.item.cellsPerRow : 0 : 0
            folded: body.item ? foldable ? body.item.folded : false : false
            onClicked: if(foldable && body.item) body.item.folded = !body.item.folded
            moving: flickerMoving

            icon: body.iconHint
            label: body.name
        }

        model: lensView.model != undefined ? lensView.model.categories : undefined
    }
}
