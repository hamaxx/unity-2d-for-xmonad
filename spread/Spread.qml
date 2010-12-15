import Qt 4.7
import UnityApplications 1.0
import UnityPlaces 1.0

Item {
    width: desktop.width
    height: desktop.height

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
