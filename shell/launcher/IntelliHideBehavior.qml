import QtQuick 1.0
import Unity2d 1.0

Item {
    id: intellihide

    property variant launcher: null
    property bool shown: true

    Timer {
        id: edgeHitTimer
        interval: 500
        onTriggered: shown = true
    }

    Connections {
        target: (launcher !== undefined) ? launcher : null
        onOuterEdgeContainsMouseChanged: edgeHitTimer.running = outerEdgeContainsMouse
        ignoreUnknownSignals: true
    }

    WindowsIntersectMonitor {
        id: windows
        monitoredArea: launcher ? Qt.rect(0, launcher.y, launcher.width, launcher.height)
                                : Qt.rect(0, 0, 0, 0)
    }

    /* For some reason this requires a Binding, assigning directly to the property
       won't work in certain cases. */
    Binding {
        target: intellihide
        property: "shown"
        value: launcher ? launcher.containsMouse || !windows.intersects : true
    }
}
