import Qt 4.7

/* This is our main view.
   It positions the shots in two very different layouts depending on its state.

   When the state is the default state (named ""), the shots will be positioned
   and scaled according to screen mode (i.e. perfectly matching the real windows).
   The positioning in screen mode only uses the real size and position taken
   from each item in the model. See the assignement of x,y,width,height and z
   and that's all you need to know about screen mode.

   When the state is the "spread" state, the shots will be positioned in a layout
   that's pretty much the same as a standard Grid, except for some peculiarities.
   More specifically, the logic for the grid is as follows:

   * The number of rows and columns depends on the total number of items (see
     exact formulas below, lifted straight from Unity's code)
   * All cells in a row have always the same width
   * The combined width of all cells in a row always equals the width of the row.
   * If there would be empty cells in the last row of the grid, then the last row
     will have less cells so that no empty cells can exist.

   The calculations are in part here and in part in SpreadItem, according to where
   it was most efficient to have them.

   The rest of the discussion below assumes that everything is referred to
   working in grid mode, unless otherwise specified explicitly.
*/
Item {
    id: list
    anchors.fill: parent

    /* The model feeds directly the Repeater, which generates and destroys
       SpreadItems according to what's in the model at any given time.
       NOTE: the content of the model changes only in response to calling its
       load() and unload() methods (see the main Spread.qml) */
    property alias items: repeater.model

    /* Calculate the number of columns and rows based on the total number
       of items. Formulas are liften straight from Unity. */
    property int count: (items) ? items.count : 0
    property int columns: Math.ceil (Math.sqrt (count))
    property int rows: Math.ceil(count / list.columns)

    /* Calculate if the number of cells in the last row (as described above) */
    property int lastRowCells: columns - ((rows * columns) - count)

    /* This is emitted when the outro animation is fully done. */
    signal spreadFinished
    property int finishedChildCount: 0 // read the HACK note below

    Repeater {
        id: repeater

        /* The "item" property of the current model value represents a
           WindowInfo object, holding coordinates and size of a real window. */
        delegate: SpreadItem {

            /* The following group of properties is the only thing needed to position
               this item in screen mode (almost exactly where the window is).
               Note that we subtract the desktop x and y since item.location is
               expressed in whole screen coordinates, but we are operating using
               only the available space on desktop (which is what desktop is). */
            x: item.location.x - desktop.x
            y: item.location.y - desktop.y
            width: item.size.width
            height: item.size.height
            z: item.z

            /* Decide in which cell of the grid this item should position itself in
               (based on the index of the current model value) */
            column: index % list.columns
            row: Math.floor(index / list.columns)

            /* Calculate height and widht of the current cell. See header for details. */
            cellHeight: (list.height - list.anchors.margins) / rows
            cellWidth: {
                var cellsInRow = (row == list.rows - 1 && list.lastRowCells != 0) ?
                        list.lastRowCells : list.columns;
                return (list.width - list.anchors.margins) / cellsInRow
            }

            /* Pass on a few properties so that they can be reference from inside the
               SpreadItem itself. The state is particularly important as it drives
               all the animations that make up intro and outro (see SpreadItem.qml) */
            state: list.state
            win: item

            /* Change the global state of the list to trigger the outro animation on
               this signal (emitted when clicking an item in the grid) */
            onSpeadShouldEnd: list.state = ""

            /* HACK: This is an hack that is needed to ensure that we consider the
               spread finished when the animation of all the items back into
               their starting positions is complete.
               Attaching to onStateChange of the SpreadGrid component itself will not
               work since the state change is triggered immediately, and the
               animations of the children will still be running. */
            onOutroFinished: {
                list.finishedChildCount++
                if (list.finishedChildCount == list.count) {
                    list.finishedChildCount = 0
                    list.spreadFinished()
                }
            }
        }
    }
}
