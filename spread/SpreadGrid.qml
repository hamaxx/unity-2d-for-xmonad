import Qt 4.7

/* This is our main view.
   It positions the shots in two very different layouts depending on its state.

   When the state is the default state (named ""), the shots will be positioned
   and scaled according to screen mode (i.e. perfectly matching the real windows).

   When the state is the "spread" state, the shots will be positioned in layout
   that's pretty much the same as a standard Grid, except for some peculiarities
   on the size of the columns in the last row in certain cases, and a certain
   oddness in margin calculations.

   The calculations are in part here and in part in SpreadItem, according to where
   it was most efficient to have them. You should probably read the two files
   together.

   IMPORTANT!
   The positioning in screen mode only uses the real size and position taken
   from each item in the model. See the assignement of x,y,width,height and z
   and that's all you need to know about screen mode.

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

    /* The pecuilarity of this grid mentioned above is this: if there would be empty
       cells in the last row of the grid, then last row will have less cells than
       normal (exactly enough to fit all the items). But they will be longer so to
       fill the entire row anyway (see the calculation for SpreadItem.columnWidth */
    property int lastRowColumns: columns - ((rows * columns) - count)

    // This is emitted when the outro animation is fully done.
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

            /* Set in which cell of the grid this item should position itself in
               (based on the index of the current model value) */
            column: index % list.columns
            row: Math.floor(index / list.columns)

            /* Shortcut to know how many cells are there in the row where we
               should position ourselves. */
            columnsInRow: (row == list.rows - 1 && list.lastRowColumns != 0) ?
                          list.lastRowColumns : list.columns

            /* Calculate height and widht of the current cell.
               The rule is that all cells in a row have always the same width
               and together always fill the entire width of the row. */
            columnWidth: (list.width - list.anchors.margins) / columnsInRow
            rowHeight: (list.height - list.anchors.margins) / rows

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
            onFinished: {
                list.finishedChildCount++
                if (list.finishedChildCount == list.count) {
                    list.finishedChildCount = 0
                    list.spreadFinished()
                }
            }
        }
    }
}
