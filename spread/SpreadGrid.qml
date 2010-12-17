import Qt 4.7

Item {
    id: list
    anchors.fill: parent
    anchors.margins: 0

    property alias items: repeater.model
    property int count: (items) ? items.count : 0
    property int columns: Math.ceil (Math.sqrt (count))
    property int rows: Math.ceil(count / list.columns)
    property int lastRowColumns: columns - ((rows * columns) - count)
    property real ratio: width / desktop.width

    signal spreadFinished
    property int finishedChildCount: 0

    Repeater {
        id: repeater

        delegate: SpreadItem {
            column: index % list.columns
            row: Math.floor(index / list.columns)
            columnsInRow: (row == list.rows - 1 && list.lastRowColumns != 0) ?
                          list.lastRowColumns : list.columns
            columnWidth: (list.width - list.anchors.margins) / columnsInRow

            property bool active: false
            state: list.state

            win: item
            ratio: list.ratio

            onFinished: {
                /* This is an hack that is needed to ensure that we consider the
                   spread finished when the animation of all the items back into
                   their starting positions is complete.
                   Attaching to onStateChange of this component itself will not
                   work since the state change is triggered immediately, and the
                   animations of the children will still be running. */
                list.finishedChildCount++
                if (list.finishedChildCount == list.count) {
                    list.finishedChildCount = 0
                    list.spreadFinished()
                }
            }
        }
    }
}
