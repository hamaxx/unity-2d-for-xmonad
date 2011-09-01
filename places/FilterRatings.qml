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

    height: rating.height

    RatingStars {
        id: rating

        height: childrenRect.height

        focus: true
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: parent.top

        size: 5
        spacing: 7
        rating: filterView.filterModel.rating
        ratingVisible: filterView.filterModel.filtering
        Binding {
            target: filterView.filterModel
            property: "rating"
            value: rating.rating
        }

        enabled: true
    }
}
