import Qt 4.7
import UnityApplications 1.0
import UnityPlaces 1.0

Rectangle {
    id: switcher

    width: availableGeometry.width
    height: availableGeometry.height

    color: "black"

    property int workspaces: 3
    property int columns: Math.ceil(Math.sqrt(workspaces))
    property int rows: columns

    /* These values are completely random. FIXME: pull from unity the proper ones */
    property int leftMargin: 40
    property int rightMargin: 40
    property int topMargin: 30
    property int spacing: 25

    /* FIXME: cell scale isn't correct in case the workspaces layout is taller than wider */
    property int availableWidth: switcher.width - ((columns - 1) * spacing) - rightMargin - leftMargin
    property real cellScale: availableWidth / columns / switcher.width
    property real zoomedScale: availableWidth / switcher.width

    property variant zoomedWorkspace

    Repeater {
        model: switcher.workspaces
        delegate: Workspace {
            id: workspace

            row: Math.floor(index / columns)
            column: index % columns

            x: column * (switcher.width * cellScale) + (column * switcher.spacing)
            y: row * (switcher.height * cellScale) + (row * switcher.spacing)
            scale:  switcher.cellScale
       }
    }
}

