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

/* Renderers typically use a grid layout to render the model. The RendererGrid
   component provides a standard implementation of such a layout where the
   cells can be customized by providing a QML component to it.
   A user of RendererGrid would create a renderer inheriting from it
   and pass a Component via the 'cellRenderer' property.
*/
Renderer {
    id: renderer

    needHeader: true
    currentItem: focusPath.currentItem

    property int contentHeight: grid.height + grid.anchors.topMargin + grid.anchors.bottomMargin
    property alias cellsPerRow: grid.columns
    property variant cellRenderer
    property bool folded: true
    property int cellWidth: 158
    property int cellHeight: 76

    property int minHorizontalSpacing: 26
    property int minVerticalSpacing: 26

    property bool centered: true

    function focusFirstElement() {
        focusPath.reset()
    }

    function focusLastRow() {
        focusPath.focusLastRow()
        focusPath.currentItem.forceActiveFocus()
    }

    FocusPath {
        id: focusPath

        item: grid
        columns: grid.columns
    }

    Grid {
        id: grid

        columns: Math.floor(parent.width/(renderer.cellWidth + renderer.minHorizontalSpacing))
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottomMargin: 12
        anchors.leftMargin: 2

        property int itemHorizontalSpacing: renderer.centered ? Math.floor(renderer.width / columns - renderer.cellWidth) : renderer.minHorizontalSpacing
        Repeater {
            id: results

            FocusPath.skip: true

            FocusScope {
                id: cell

                width: renderer.cellWidth + grid.itemHorizontalSpacing
                height: renderer.cellHeight + renderer.minVerticalSpacing
                /* When hovered the item needs to be on top of every other item
                   in order for its label to not be covered */
                z: ( loader.item.state == "selected" || loader.item.state == "hovered" ) ? 1 : 0

                FocusPath.index: index

                Loader {
                    id: loader

                    focus: true
                    width: renderer.cellWidth
                    height: renderer.cellHeight
                    anchors.horizontalCenter: parent.horizontalCenter
                    anchors.verticalCenter: parent.verticalCenter

                    sourceComponent: cellRenderer
                    onLoaded: {
                        item.uri = column_0
                        item.iconHint = column_1
                        item.mimetype = column_3
                        item.displayName = column_4
                        item.comment = column_5
                        item.focus = true
                        item.dndUri = column_6
                    }
                }
            }

            /* Only display one line of items when folded */
            model: SortFilterProxyModel {
                model: renderer.category_model != undefined ? renderer.category_model : null
                limit: renderer.folded ? grid.columns : -1
            }
        }
    }
}
