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

    property int selectedXid: 0
    property variant orderedXids: []

    function addXidAt(index, xid) {
        /* Here we can't assign directly use orderedXids[index] = window
           because, due to restrictions in the way list properties are implemented
           in QML, reading from a list-type property returns *a copy* of the list.
           Therefore we take that copy, add the item, then reassign it to the property.
           See: http://doc.qt.nokia.com/4.7-snapshot/qml-list.html
        */
        var copy = orderedXids;
        copy[index] = xid
        orderedXids = copy
    }
    function removeXidAt(index) {
        var copy = orderedXids;
        copy.splice(index, 1)
        orderedXids = copy
    }

    function unload() {
        orderedXids = []
    }

    /* Given row and column, calculate the index of the SpreadWindow in the orderedXids list
       and then ask to make it the globally selected window. */
    function selectAt(row, column) {
        var index = row * layout.columns + column
        selectXid(orderedXids[index])
    }

    Keys.onPressed: if (handleKeyPress(event.key)) event.accepted = true
    function handleKeyPress(key) {
        switch (key) {
        case Qt.Key_Enter:
        case Qt.Key_Return:
            if (navigator.selectedXid != 0) layout.windowActivated()
            return true

        case Qt.Key_Right:
        case Qt.Key_Left:
        case Qt.Key_Up:
        case Qt.Key_Down:
            if (navigator.selectedXid == 0) {
                selectAt(0,0)
                return true
            }

            var at = spread.windowForXid(navigator.selectedXid)
            if (at == null) return true

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

    function selectXid(requestedXid) {
        if (requestedXid == navigator.selectedXid) return

        var selectedWindow = spread.windowForXid(navigator.selectedXid)
        var requestedWindow = (requestedXid == 0) ? null : spread.windowForXid(requestedXid)

        if (selectedWindow) selectedWindow.isSelected = false
        if (requestedWindow) requestedWindow.isSelected = true
        navigator.selectedXid = requestedXid
    }

    function selectWindowByWindowInfo(selected) {
        if (!selected) return;
        for (var i = 0; i < orderedXids.length; i++) {
            if (orderedXids[i].windowInfo.xid == selected.xid) {
                selectWindow(orderedXids[i])
                return
            }
        }
    }
}
