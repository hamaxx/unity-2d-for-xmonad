import Qt 4.7

BorderImage {
    source: "artwork/border_glow.png"
    smooth: false
    border.left: 11
    border.right: 11
    border.top: 12
    border.bottom: 12
    /* It might be more efficient to have a png with a bigger transparent
       middle and setting the tile modes to Repeat */
    horizontalTileMode: BorderImage.Stretch
    verticalTileMode: BorderImage.Stretch

    Image {
        anchors.fill: parent
        anchors.margins: 6
        source: "artwork/checker.png"
        fillMode: Image.Tile
    }
}
