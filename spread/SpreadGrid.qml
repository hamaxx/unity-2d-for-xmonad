import Qt 4.7

Item {
    id: list
    anchors.fill: parent
    anchors.margins: 0

    property alias items: repeater.model
    property int count: items.count
    property int columns: Math.ceil (Math.sqrt (count))
    property int rows: Math.ceil(count / list.columns)
    property int lastRowColumns: (rows * columns) - count
    property real ratio: width / desktop.width

    Repeater {
        id: repeater

        delegate: SpreadItem {
            column: index % list.columns
            row: Math.floor(index / list.columns)

            property bool active: false
            state: parent.state

            win: item

            onNeedsActivationChanged: if (needsActivation) parent.state = ""
        }
    }
}
