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
import Effects 1.0

Item {
    /* Fill is between 0 and 1 */
    property real fill: 0
    property int iconSize: 32
    property bool selected: false
    property alias ratingVisible: ratingStar.visible

    width: childrenRect.width
    height: childrenRect.height

    effect: DropShadow {
         blurRadius: 8
         color: "white"
         offset.x: 0
         offset.y: 0
    }

    Image {
        width: sourceSize.width
        height: sourceSize.height

        source: ("artwork/star_empty-%1.png").arg(iconSize)
        opacity: ( selected ) ? 0.8 : 0.3
        asynchronous: true
    }

    Image {
        id: ratingStar
        width: Math.ceil(sourceSize.width * fill)
        height: sourceSize.height

        source: ("artwork/star_full-%1.png").arg(iconSize)
        fillMode: Image.TileHorizontally
        asynchronous: true
        opacity: ( selected ) ? 1 : 0.8
        clip: true
    }
}


