import QtQuick 1.0

Item {
    id: ratingStars

    property real rating: 3.7
    property int size: 5 /* Number of stars */
    property alias enabled: starsMouseArea.enabled
    property alias spacing: stars.spacing

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
        if ( rating % 1 > 0){ /* if non-integer, round up */
            rating = Math.ceil(rating)
        }
        else {
            rating = clamp(rating+1, 0, size)
        }
        return true
    }

    function decrementRating() {
        if ( rating % 1 > 0){ /* if non-integer, round down */
            rating = Math.floor(rating)
        }
        else {
            rating = clamp(rating-1, 0, size)
        }
        return true
    }

    Keys.onPressed: if (handleKeyPress(event.key)) event.accepted = true
    function handleKeyPress(key) {
        switch (key) {
        case Qt.Key_Right:
            return incrementRating()
        case Qt.Key_Left:
            return decrementRating()
        }
        return false
    }

    Row {
        id: stars

        property real currentRating: rating
        Repeater {
            model: size
            Star {
                fill: clamp(stars.currentRating - index, 0, 1)
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
            /* Mouse X coordinate over one unit, relative to that unit's left edge*/
            var posXOverUnit = posX % unitWidth

            /* What unit is the mouse over? */
            var mouseOverUnitNumber = (posX - posXOverUnit) / unitWidth

            /* If mouse is over the star icon part of the unit */
            if ( posXOverUnit <= starIconSize ){
                return clamp ( (posX - mouseOverUnitNumber*stars.spacing)/starIconSize , 0, size)
            }
            /* Else mouse is over gap */
            else{
                return mouseOverUnitNumber+1
            }
        }

        property bool mouseDown: false
        anchors.fill: stars

        onPressed: {
            mouseDown = true;
            stars.currentRating = calculateRating(mouseX)
        }
        onPositionChanged: {
            if (mouseDown) stars.currentRating = calculateRating(mouseX)
        }
        onReleased: {
            mouseDown = false;
            rating = stars.currentRating
        }
    }
}
