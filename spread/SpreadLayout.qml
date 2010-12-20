import Qt 4.7

/* This is our main view.
   It positions the windows in two very different layouts depending on its state.

   When the state is the default state (named ""), the windows will be positioned
   and scaled according to screen mode (i.e. perfectly matching the real windows).
   The positioning in screen mode only uses the real size and position taken
   from each window in the model.

   When the state is the "spread" state, the windows will be positioned in a layout
   that is pretty much the same as a standard GridView, except for some peculiarities.
   More specifically, the logic for the spread is as follows:

   * The number of rows and columns depends on the total number of windows (see
     exact formulas below)
   * The width of a row is split equally among the its cells
   * If there would be empty cells in the last row of the spread, then the last row
     will have less cells so that no empty cells can exist.
*/
Item {
    id: layout

    property alias windows: repeater.model

    /* Number of columns and rows based on the total number of windows.
       Formulas are lifted straight from Unity. */
    property int count: windows ? windows.count : 0
    property int columns: Math.ceil (Math.sqrt (count))
    property int rows: Math.ceil(count / layout.columns)

    /* Number of cells in the last row: (as described above) */
    property int lastRowCells: count - (columns * (rows - 1))

    /* After any state change wait for transitionDuration (ms) and then emit
       the signal transitionCompleted */
    property int transitionDuration: 250
    signal transitionCompleted

    transitions: Transition {
        ScriptAction { script: transitionTimer.restart() }
    }

    Timer {
        id: transitionTimer

        interval: transitionDuration
        onTriggered: transitionCompleted()
    }


    Repeater {
        id: repeater

        delegate: SpreadWindow {
            transitionDuration: layout.transitionDuration

            /* The following group of properties is the only thing needed to position
               this window in screen mode (almost exactly where the window is).
               Note that we subtract the availableGeometry x and y since window.location is
               expressed in screen coordinates. */
            x: window.position.x - availableGeometry.x
            y: window.position.y - availableGeometry.y
            width: window.size.width
            height: window.size.height
            z: window.z

            /* Decide in which cell of the layout this window should position itself in
               (based on the index of the current model value) */
            column: index % layout.columns
            row: Math.floor(index / layout.columns)

            /* Height and width of the current cell. See header for details. */
            cellHeight: (layout.height - layout.anchors.margins) / rows
            cellWidth: {
                var cellsInRow = (row == layout.rows - 1 && layout.lastRowCells != 0) ?
                        layout.lastRowCells : layout.columns;
                return (layout.width - layout.anchors.margins) / cellsInRow
            }

            /* Pass on a few properties so that they can be referenced from inside the
               SpreadWindow itself. The state is particularly important as it drives
               all the animations that make up intro and outro */
            state: layout.state
            windowInfo: window

            onClicked: {
                /* This is a bit of an hack, to raise the window above all the others
                   before starting the outro, but it's ok since at the end of the outro
                   all the windows will be unloaded (and we will never have >9999 windows) */
                z = 9999
                /* Immediately activate the window. Since spread is an
                   always-on-top window, we can raise the window now so it
                   will be already in the correct position when the outro finishes */
                windowInfo.activate()
                layout.state = ""
            }
        }
    }
}
