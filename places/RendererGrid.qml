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
    width: parent.width
    height:  grid.height
    needHeader: true

    property alias cellsPerRow: grid.columns
    property variant cellRenderer
    property bool folded: true
    property int cellWidth: 158
    property int cellHeight: 76

    /* used for celtralized grid */
    property int minHorizontalSpacing: 0
    property int minVerticalSpacing: 0

    property int horizontalSpacing: 26
    property int verticalSpacing: 26

    property bool centralized: true

    FocusPath {
        id: focusPath
        item: grid
        columns: grid.columns
    }

    onActiveFocusChanged: {
        if (activeFocus && (grid.children > focusPath.currentIndex)) {
            currentItem = grid.children[focusPath.currentIndex]
        }
    }

    Grid {
        id: grid
        columns: Math.floor(parent.width/(renderer.cellWidth + renderer.horizontalSpacing))
        anchors.topMargin: 12
        width: parent.width

        property int virtualHorizontalSpacing: renderer.centralized ? Math.floor(renderer.width / columns - renderer.cellWidth) : renderer.horizontalSpacing
        /* Keep the horizontal space alway larger then minHorizontalSpacing */
        property int itemHorizontalSpacing: virtualHorizontalSpacing < minHorizontalSpacing ? minHorizontalSpacing : virtualHorizontalSpacing
        Repeater {
            id: results
            FocusPath.skip: true

            FocusScope {
                id: cell
                width: renderer.cellWidth + grid.itemHorizontalSpacing
                height: renderer.cellHeight + renderer.verticalSpacing
                FocusPath.index: index

                property string uri: column_0
                property string iconHint: column_1
                property string categoryId: column_2 // FIXME: rename to categoryIndex
                property string mimetype: column_3
                property string displayName: column_4 // FIXME: rename to name
                property string comment: column_5
                property string dndUri: column_6

                onActiveFocusChanged: {
                    if (activeFocus) {
                        renderer.currentItem = cell
                    }
                }

                Loader {
                    id: loader
                    focus: true
                    clip: true
                    width: renderer.cellWidth
                    height: renderer.cellHeight
                    anchors.horizontalCenter: parent.horizontalCenter
                    anchors.verticalCenter: parent.verticalCenter

                    sourceComponent: cellRenderer
                    onLoaded: {
                        item.uri = uri
                        item.iconHint = iconHint
                        item.mimetype = mimetype
                        item.displayName = displayName
                        item.comment = comment
                        item.focus = true
                        item.dndUri = dndUri
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
