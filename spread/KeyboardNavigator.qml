import Qt 4.7

/* This component groups together all the keboard handling
   for the SpreadLayout, but more importantly it does also take care of the
   logic deciding which item in the spread will be selected in response to a
   cursor key press.

   The rules for keyboard interaction are the following:

   - Keyboard interaction is enabled only in spread mode.
   - If there's no window selected, pressing any cursor key will select the item
     at the top-left corner.
   - If there's a selected window, then the cursor keys will move selection to the
     neighbour element in that direction, wrapping at all edges.
   - The ESC key will cancel any selection and trigger a spread outro.
   - The ENTER key trigger a spread outro and cause the currently selected window
     to activate (as if it was clicked).

   We don't perform any of the actions mentioned above, we just emit signals and let
   the SpreadLayout take care of everything else.
*/


Item {
    id: navigator

    property variant selectedWindow

    signal selectionRequested(variant newSelection)
    signal exitRequested()

    property variant orderedWindows: []
    function addWindowAt(index, window) {
        /* Here we can't assign directly use orderedWindows[index] = window
           because, due to restrictions in the way list properties are implemented
           in QML, reading from a list-type property returns *a copy* of the list.
           Therefore we take that copy, add the item, then reassign it to the property.
           See: http://doc.qt.nokia.com/4.7-snapshot/qml-list.html
        */
        var copy = orderedWindows;
        copy[index] = window
        orderedWindows = copy
    }

    function unload() {
        orderedWindows = []
    }

    /* Given row and column, calculate the index of the SpreadWindow in the orderedWindows list
       and then emit a signal to notify the layout, which will use its own logic to choose the
       globally selected window. */
    function selectAt(row, column) {
        var index = row * layout.columns + column
        selectionRequested(orderedWindows[index])
    }

    Keys.onPressed: if (handleKeyPress(event.key)) event.accepted = true
    function handleKeyPress(key) {
        switch (key) {
        case Qt.Key_Escape:
            /* This will cancel the selection, so at the end of the
               outro no window will be activated */
            selectionRequested(null)
            exitRequested()
            return true

        case Qt.Key_Enter:
        case Qt.Key_Return:
            if (selectedWindow) exitRequested()
            return true

        case Qt.Key_Right:
        case Qt.Key_Left:
        case Qt.Key_Up:
        case Qt.Key_Down:
            if (!selectedWindow) {
                selectAt(0,0)
                return true
            }

            var at = selectedWindow

            switch (key) {
            case Qt.Key_Right:
                if (at.column < layout.columnsInRow(at.row) - 1) selectAt(at.row, at.column + 1)
                else selectAt(at.row, 0)
                return true

            case Qt.Key_Left:
                if (at.column > 0) selectAt(at.row, at.column - 1)
                else selectAt(at.row, layout.columnsInRow(at.row) - 1)
                return true

            case Qt.Key_Up:
                /* If we're going up, and we're on the first row, then we wrap to the last row.
                   However it may have less columns than the others, so we need to make sure we
                   clamp the destination column to an existing one. */
                if (at.row > 0) selectAt(at.row - 1, at.column)
                else selectAt(layout.rows - 1, Math.min(at.column, layout.lastRowCells - 1))
                return true

            case Qt.Key_Down:
                /* When we're going down, if we are on the last row, we just wrap to the first.
                   If we are on the row before the last row, we need to consider that the last row
                   my have less columns than the current one and clamp the destination column. */
                if (at.row == layout.rows - 2) selectAt(at.row + 1, Math.min(at.column, layout.lastRowCells - 1))
                else if (at.row == layout.rows - 1) selectAt(0, at.column)
                else selectAt(at.row + 1, at.column)
                return true
            }
        }

        return false
    }
}
