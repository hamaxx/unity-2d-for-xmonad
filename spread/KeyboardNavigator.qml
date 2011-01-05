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
*/

Item {
    id: navigator

    property variant selectedWindow

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
       and then ask to make it the globally selected window. */
    function selectAt(row, column) {
        var index = row * layout.columns + column
        selectWindow(orderedWindows[index])
    }

    Keys.onPressed: if (handleKeyPress(event.key)) event.accepted = true
    function handleKeyPress(key) {
        switch (key) {
        case Qt.Key_Escape:
            /* This will cancel the selection, so at the end of the
               outro no window will be activated */
            selectWindow(null)
            layout.exitSpread()
            return true

        case Qt.Key_Enter:
        case Qt.Key_Return:
            if (selectedWindow) layout.exitSpread()
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
                selectAt(at.row, (at.column + 1) % layout.columnsInRow(at.row))
                return true

            case Qt.Key_Left:
                selectAt(at.row, (at.column > 0) ? (at.column - 1) : (layout.columnsInRow(at.row) - 1))
                return true

            case Qt.Key_Up:
                /* If we're going up, and we're on the first row, then we wrap to the last row.
                   However it may have less columns than the others, so we need to make sure we
                   clamp the destination column to an existing one. */
                var targetRow = (at.row > 0) ? (at.row - 1) : (layout.rows - 1)
                selectAt(targetRow, Math.min(at.column, layout.columnsInRow(targetRow) - 1))
                return true

            case Qt.Key_Down:
                /* When we're going down, if we are on the last row, we just wrap to the first.
                   If we are on the row before the last row, we need to consider that the last row
                   my have less columns than the current one and clamp the destination column. */
                var targetRow = (at.row + 1) % layout.rows
                selectAt(targetRow, Math.min(at.column, layout.columnsInRow(targetRow) - 1))
                return true
            }
        }

        return false
    }

    function selectWindow(selected) {
        if (selectedWindow) selectedWindow.isSelected = false
        if (selected) selected.isSelected = true
        selectedWindow = selected
    }
}
