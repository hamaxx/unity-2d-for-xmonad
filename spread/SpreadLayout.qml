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

    /* There can be only one (or none) selected window at the same time and
       selection needs to be possible with both the keyboard and the mouse.
       Therefore we just maintain the current selection globally at the SpreadLayout
       level, and update it based on selection events from the MouseArea
       inside each SpreadWindow and from the KeyboardNavigator */
    property variant selectedWindow
    function select(selected) {
        if (selectedWindow) selectedWindow.isSelected = false
        if (selected) selected.isSelected = true
        selectedWindow = selected
    }

    function exitSpread() {
        /* This is a bit of an hack, to raise the window above all the others
           before starting the outro, but it's ok since at the end of the outro
           all the windows will be unloaded (and we will never have >9999 windows) */
        if (layout.selectedWindow) layout.selectedWindow.z = 9999
        layout.state = ""
    }

    transitions: Transition {
        ScriptAction { script: transitionTimer.restart() }
    }

    Timer {
        id: transitionTimer

        interval: transitionDuration
        onTriggered: transitionCompleted()
    }

    /* This component handles all the logic related to navigation with the
       keyboard inside the SpreadLayout when we are in spread mode. */
    KeyboardNavigator {
        id: navigator

        /* Disable keyboard focus when not in spread mode to prevent accidental
           keypresses from messing up the selection. */
        focus: layout.state == "spread"

        /* The keyboard navigator needs to know what is the currently global
           selected window (to know what is the next one to select in reaction to
           cursor key navigation) */
        selectedWindow: layout.selectedWindow
        onSelectionChanged: layout.select(newSelection)
        onExitRequested: layout.exitSpread()

        /* It is very important to clean up the internal state of the
           navigator when a spread is completed, otherwise it will keep
           un-needed references to SpreadWindow instances.

           NOTE: it is safe to use this property change notification for this because
           due to the way the WindowsList's load() and unload() methods work, the count
           value is either zero or the full count of the items in the model. It
           can never take any other value. */
        Connections {
            target: layout.windows
            onCountChanged: if (count == 0) navigator.unload()
        }
    }

    Repeater {
        id: repeater

        delegate: SpreadWindow {
            id: spreadWindow
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

            onExitRequested: layout.exitSpread()
            onSelectionChanged: layout.select((selected) ? spreadWindow : null)

            /* This is a workaround for an issue with how QML handles the "children"
               property.
               According to the documentation children are *inserted* in order into their
               parent's children list (in our case they are inserted into the SpreadLayout
               by the Repeater).
               However they are apparently not *maintained* in the same order.
               In other words, the list of children can be re-arranged whenver the parent
               feels like, with no rules that I could find documented anywhere.
               Since we need an ordered list for keyboard navigation, we need to maintain
               it ourselves. */
            Component.onCompleted: navigator.addWindowAt(index, spreadWindow)
        }
    }
}
