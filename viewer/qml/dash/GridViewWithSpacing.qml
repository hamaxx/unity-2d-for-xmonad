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

GridView {
    property int horizontalSpacing: 0
    property int verticalSpacing: 0
    property int columns: 2
    property int rows: 2

    function delegateX(column) {
        return column * horizontalSpacing / columns
    }
    function delegateY(row) {
        return row * verticalSpacing / rows
    }

    property int delegateWidth: cellWidth - horizontalSpacing + horizontalSpacing / columns
    property int delegateHeight: cellHeight - verticalSpacing + verticalSpacing / rows
    cellWidth: delegateWidth + horizontalSpacing - horizontalSpacing / rows
    cellHeight: delegateHeight + verticalSpacing - verticalSpacing / rows
}
