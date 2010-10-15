import Qt 4.7

BorderImage {
    source: "artwork/border_glow.png"
    smooth: false
    border.left: 11
    border.right: 11
    border.top: 12
    border.bottom: 12
    horizontalTileMode: BorderImage.Stretch
    verticalTileMode: BorderImage.Stretch

    Image {
        anchors.fill: parent
        anchors.margins: 6
        source: "artwork/checker.png"
        fillMode: Image.Tile
    }
}
