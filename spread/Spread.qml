import Qt 4.7
import UnityApplications 1.0

Item {
    width: 1000
    height: 800

    MouseArea {
        anchors.fill: parent
        onClicked: {
            grid.state = (grid.state == "spread") ? "screen" : "spread"
        }
    }

    WindowsList {
        id: wins
    }

    SpreadGrid {
        id: grid
        items: wins
    }

    Component.onCompleted: {
        grid.state = "spread"
    }
}
