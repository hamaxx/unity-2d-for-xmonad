import QtQuick 1.0
import Effects 1.0

Item {
    /* Fill is between 0 and 1 */
    property real fill: 0
    property int iconSize: 32
    property bool selected: false

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
        width: Math.ceil(sourceSize.width * fill)
        height: sourceSize.height

        source: ("artwork/star_full-%1.png").arg(iconSize)
        fillMode: Image.TileHorizontally
        asynchronous: true
        opacity: ( selected ) ? 1 : 0.8
        clip: true
    }
}


