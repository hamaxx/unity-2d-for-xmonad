import Qt 4.7

BorderImage {
    source: "artwork/border_glow.sci"
    smooth: false
    /* It might be more efficient to have a png with a bigger transparent
       middle and setting the tile modes to Repeat */
    horizontalTileMode: BorderImage.Stretch
    verticalTileMode: BorderImage.Stretch
}
