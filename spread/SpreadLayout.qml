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

    /* Number of cells in the last row: (as described above) and small
       convenience function to quickly know how many cells are in any
       given row. */
    property int lastRowCells: count - (columns * (rows - 1))
    function columnsInRow(row) {
        return (row == layout.rows - 1 && layout.lastRowCells != 0) ?
                layout.lastRowCells : layout.columns;
    }

    signal windowActivated

    /* We need to make this information available to the parent, so that it
       knows which window to activate at the end of the spread (if any) */
    property alias selectedXid: navigator.selectedXid

    function raiseSelectedWindow() {
        /* This is a bit of an hack, to raise the window above all the others
           before starting the outro, but it's ok since at the end of the outro
           all the windows will be unloaded (and we will never have >9999 windows) */
        if (navigator.selectedXid != 0) {
            var selectedWindow = spread.windowForXid(navigator.selectedXid)
            if (selectedWindow) selectedWindow.z = 9999
        }
    }

    transitions: Transition {
        ScriptAction { script: transitionTimer.restart() }
    }

    Timer {
        id: transitionTimer

        interval: switcher.transitionDuration
        onTriggered: {
            if (state == "spread")
                navigator.selectWindowByWindowInfo(windows.lastActiveWindow)
        }
    }

    /* This component handles all the logic related to selection and to
       navigation with the keyboard while we are in spread mode. */
    KeyboardNavigator {
        id: navigator
        focus: workspace.isZoomed
    }

    Repeater {
        id: repeater

        delegate: SpreadWindow {
            id: spreadWindow
            transitionDuration: switcher.currentTransitionDuration

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
            cellWidth: (layout.width - layout.anchors.margins) / columnsInRow(row)

            /* Pass on a few properties so that they can be referenced from inside the
               SpreadWindow itself. The state is particularly important as it drives
               all the animations that make up intro and outro */
            state: layout.state
            windowInfo: window

            onClicked: {
                navigator.selectXid(spreadWindow.windowInfo.contentXid)
                layout.windowActivated()
            }
            onExited: if (navigator.selectedXid == spreadWindow.windowInfo.contentXid)
                          navigator.selectXid(0)
            onEntered: navigator.selectXid(spreadWindow.windowInfo.contentXid)

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
            Component.onCompleted: navigator.addXidAt(index, spreadWindow.windowInfo.contentXid)
            Component.onDestruction: navigator.removeXidAt(index)
        }
    }
}
