import QtQuick 1.0

Item {
    id: autoHide
    property bool shown: true
    property variant launcher: null

    Timer {
        id: autoHideTimer
        interval: 1000
        running: !launcher.containsMouse
        onTriggered: shown = false
    }

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
}
