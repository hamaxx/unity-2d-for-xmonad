import Qt 4.7

Item {
    id: list
    anchors.fill: parent
    anchors.margins: 0

    property alias items: repeater.model
    property int count: items.count
    property int columns: Math.ceil (Math.sqrt (count))
    property int rows: Math.ceil(count / list.columns)
    property int gap: (rows * columns) - count
    property real ratio: width / screen.width

    state: "screen"

    Repeater {
        id: repeater

        delegate: SpreadItem {
            column: index % list.columns
            row: Math.floor(index / list.columns)

            state: parent.state

            appName: item.appName + "(" + item.xid + ")"

            icon: "image://icons/" + item.icon
            capture: "image://window/" + item.xid
            title: item.title

            win_x: item.location.x
            win_y: item.location.y
            win_width: item.size.width
            win_height: item.size.height
            win_z: item.z
        }
    }
}
