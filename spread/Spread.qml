import Qt 4.7
import UnityApplications 1.0

Item {
    width: screen.width * 0.75
    height: screen.height * 0.75

    MouseArea {
        anchors.fill: parent
        onClicked: {
            grid.state = (grid.state == "spread") ? "" : "spread"
        }
    }

    WindowsList {
        id: wins
    }

    SpreadGrid {
        id: grid
        items: wins
    }

    Timer {
        id: delay
        interval: 450;
        running: false
        onTriggered: grid.state = "spread"
    }

    Component.onCompleted: {
        wins.setAppAsPager()
        delay.running = true
    }
}
