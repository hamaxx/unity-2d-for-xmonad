import Qt 4.7
import UnityApplications 1.0
import UnityPlaces 1.0

Rectangle {
    id: switcher
    width: availableGeometry.width
    height: availableGeometry.height

    color: "green"

    property int applicationId

    property int workspaces: 3
    property int columns: Math.ceil(Math.sqrt(workspaces))
    property int rows: columns

    property int leftMargin: 40
    property int rightMargin: 40
    property int topMargin: 30
    property int spacing: 25

    property int availableWidth: switcher.width - ((columns - 1) * spacing) - rightMargin - leftMargin
    property real cellScale: (availableWidth / columns) / switcher.width

    Repeater {
        model: switcher.workspaces
        delegate: Workspace {
            id: workspace

            state: switcher.state
            applicationId: switcher.applicationId

            row: Math.floor(index / columns)
            column: index % columns

            cellX: leftMargin + column * (switcher.width * cellScale) + (column * switcher.spacing)
            cellY: topMargin + row * (switcher.height * cellScale) + (row * switcher.spacing)
            cellScale:  switcher.cellScale

            onStateChanged: if (state == "") switcher.state = ""
       }
    }

    Connections {
        target: control

        onActivateSpread: {
            switcher.applicationId = applicationId
            switcher.state = "switching"
        }

        onCancelSpread: switcher.state = ""
    }

    Component.onCompleted: {
        console.log(width + "x" + height)
        console.log(rows + "x" + columns)
        console.log(cellScale)
    }
}

