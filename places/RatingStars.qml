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

Item {
    id: ratingStars

    property real rating
    property int size: 5 /* Number of stars */
    property alias enabled: starsMouseArea.enabled
    property alias spacing: stars.spacing
    property bool ratingVisible: true

    /* Configure icon size to use. Requires icon files named:
       - artwork/star_empty-${starIconSize}.png
       - artwork/star_full-${starIconSize}.png
    */
    property int starIconSize: 32

    width: stars.width
    height: stars.height

    function clamp(x, min, max) {
        return Math.max(Math.min(x, max), min)
    }

    function incrementRating() {
        rating = clamp(Math.floor(rating) + 1, 1, size)
    }

    function decrementRating() {
        rating = clamp(Math.ceil(rating) - 1, 1, size)
    }

    Keys.onPressed: if (handleKeyPress(event.key)) event.accepted = true
    function handleKeyPress(key) {
        switch (key) {
        case Qt.Key_Right:
            incrementRating()
            return true
        case Qt.Key_Left:
            decrementRating()
            return true
        }
        return false
    }

    Row {
        id: stars

        Repeater {
            model: size
            Star {
                fill: if(!enabled){ /* If read-only, display any Star rating */
                          return clamp(rating - index, 0, size)
                      }
                      else{ /* If user selectable, restrict to integer selections */
                          return (rating - index > 0) ? 1 : 0
                      }
                iconSize: starIconSize
                selected: ( ratingStars.activeFocus )
                ratingVisible: ( ratingStars.ratingVisible )
            }
        }
    }

    MouseArea {
        id: starsMouseArea

        /* Calculating Star Rating incorporating inter-star spacing/gap.

           Consider each star+gap as a "unit". Determine what unit the mouse is over,
           and remove the width of the gaps to the left from the reported mouse position.
          */

        /* Width of each unit */
        property int unitWidth: starIconSize + stars.spacing

        function calculateRating( posX ){
            /* Mouse X coordinate over one unit, relative to that unit's left edge*/
            var posXOverUnit = posX % unitWidth

            /* What unit is the mouse over? This is the integer part of the rating (plus one)*/
            var rating = (posX - posXOverUnit) / unitWidth + 1

            return clamp( rating, 0, size )
        }

        anchors.fill: stars

        onPressed: rating = calculateRating(mouseX)
        onPositionChanged: {
            if (pressed) rating = calculateRating(mouseX)
        }
    }
}
