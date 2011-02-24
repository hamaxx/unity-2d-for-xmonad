import Qt 4.7

GridView {
    property int minHorizontalSpacing: 0
    property int minVerticalSpacing: 0
    property int delegateWidth: 100
    property int delegateHeight: 100

    /* Compute the number of cells per row and column so that:
        - the items are always centered horizontally within the grid (= same
          margins on the right and left sides of the grid) when flow is set
          to LeftToRight
        - the items are always centered vertically within the grid (= same
          margins on the top and bottom sides of the grid) when flow is set
          to TopToBottom
        - the spacing between items is kept as small as possible but always
          greater than minHorizontalSpacing and minVerticalSpacing
    */
    property int cellsPerRow: flow == GridView.LeftToRight ? Math.floor(width/(minHorizontalSpacing+delegateWidth)) : 0
    property int cellsPerColumn: flow == GridView.LeftToRight ? 0 : Math.floor(height/(minVerticalSpacing+delegateHeight))
    property int horizontalSpacing: flow == GridView.LeftToRight ? Math.floor(width/cellsPerRow-delegateWidth) : minHorizontalSpacing
    property int verticalSpacing: flow == GridView.LeftToRight ? minVerticalSpacing : Math.floor(height/cellsPerColumn-delegateHeight)

    cellWidth: delegateWidth+horizontalSpacing
    cellHeight: delegateHeight+verticalSpacing
}
