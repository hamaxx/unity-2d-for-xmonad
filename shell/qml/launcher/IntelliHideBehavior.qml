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
        target: launcher
        onOuterEdgeContainsMouseChanged: edgeHitTimer.running = outerEdgeContainsMouse
        ignoreUnknownSignals: true
    }

    WindowsIntersectMonitor {
        id: windows
        monitoredArea: Qt.rect(0, launcher.y, launcher.width, launcher.height)
    }

    /* For some reason this requires a Binding, assigning directly to the property
       won't work in certain cases. */
    Binding {
        target: intellihide
        property: "shown"
        value: launcher.containsMouse || !windows.intersects
        when: launcher !== null
    }
}
