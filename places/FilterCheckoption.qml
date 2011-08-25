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

Filter {
    id: filterView

    height: filters.height

    GridViewWithSpacing {
        id: filters

        columns: ( filterView.filterModel.id == "genre"
                  || filterView.filterModel.id == "modified" ) ? 3 : 2
        rows: Math.ceil(count/columns)

        horizontalSpacing: 10
        verticalSpacing: 12

        cellWidth: width/columns
        delegateHeight: 30

        height: cellHeight * rows
        boundsBehavior: Flickable.StopAtBounds

        focus: true
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: parent.top

        /* Make sure the first item is selected when getting the focus for the first time */
        currentIndex: 0

        delegate: FocusScope {
            TickBox {
                focus: true
                width: filters.delegateWidth
                height: filters.delegateHeight
                x: filters.delegateX(model.index % filters.columns)
                text: item.name
                checked: item.active
                onClicked: item.active = !item.active
            }
        }

        model: filterView.filterModel.options
    }
}
