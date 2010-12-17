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

            onNeedsActivationChanged: if (needsActivation) parent.state = ""
            onItemActivationFinished: {
                control.hide()
                repeater.model.unload()
            }
        }
    }
}
