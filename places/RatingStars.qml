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

    property real rating: 3.7
    property int size: 5 /* Number of stars */
    property alias enabled: starsMouseArea.enabled
    property alias spacing: stars.spacing
    property bool hide_rating: false

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
        /* Make math easier since half rating accepted */
        var double_rating = rating * 2

        if ( double_rating % 1 > 0){ /* if non-integer, round up */
            double_rating = Math.ceil(double_rating)
        }
        else {
            double_rating = clamp(double_rating+1, 0, size * 2)
        }
        rating = double_rating / 2
    }

    function decrementRating() {
        var double_rating = rating * 2

        if ( double_rating % 1 > 0){ /* if non-integer, round down */
            double_rating = Math.ceil(double_rating)
        }
        else {
            double_rating = clamp(double_rating-1, 0, size * 2)
        }
        rating = double_rating / 2
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
                fill: ( hide_rating ) ? 0 : clamp(rating - index, 0, 1)
                iconSize: starIconSize
                selected: ( ratingStars.activeFocus )
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
            /* Small left-hand edge to set zero rating */
            if( posX < 4 ) return 0

            /* Mouse X coordinate over one unit, relative to that unit's left edge*/
            var posXOverUnit = posX % unitWidth

            /* What unit is the mouse over? This is the integer part of the rating (plus one)*/
            var rating = (posX - posXOverUnit) / unitWidth + 1

            /* If posX under half the star's width, remove 0.5 from the rating */
            if( posXOverUnit <= (starIconSize/2) ){
                rating = rating - 0.5
            }
            return clamp( rating, 0, size )
        }

        anchors.fill: stars

        onPressed: rating = calculateRating(mouseX)
        onPositionChanged: {
            if (pressed) rating = calculateRating(mouseX)
        }
    }
}
