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
    property alias cellsPerRow: results.cellsPerRow
    property alias contentY: results.contentY
    property alias currentItem: results.currentItem

    property variant cellRenderer
    property bool folded: true

    property int cellWidth: 158
    property int cellHeight: 76
    property int horizontalSpacing: 26
    property int verticalSpacing: 26

    /* FIXME: using results_layout.anchors.topMargin in the following expression
              causes QML to think they might be an anchor loop. */
    property int totalHeight: results.count > 0 ? results_layout.anchors.topMargin + results.totalHeight : 0

    Item {
        id: results_layout

        anchors.fill: parent
        anchors.topMargin: 12
        anchors.leftMargin: 2

        CenteredGridView {
            id: results

            focus: true

            anchors.fill: parent

            property int totalHeight: results.cellHeight*Math.ceil(count/cellsPerRow)

            minHorizontalSpacing: renderer.horizontalSpacing
            minVerticalSpacing: renderer.verticalSpacing
            delegateWidth: renderer.cellWidth
            delegateHeight: renderer.cellHeight

            interactive: false
            clip: true

            delegate: FocusScope {

                width: results.cellWidth
                height: results.cellHeight
                /* When hovered the item needs to be on top of every other item
                   in order for its label to not be covered */
                z: ( loader.item.state == "selected" || loader.item.state == "hovered" ) ? 1 : 0

                Loader {
                    id: loader
                    property string uri: column_0
                    property string iconHint: column_1
                    property string categoryId: column_2 // FIXME: rename to categoryIndex
                    property string mimetype: column_3
                    property string displayName: column_4 // FIXME: rename to name
                    property string comment: column_5
                    property string dndUri: column_6

                    width: results.delegateWidth
                    height: results.delegateHeight
                    anchors.horizontalCenter: parent.horizontalCenter

                    focus: true
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
                limit: folded ? results.cellsPerRow : -1
            }
        }
    }
}
