import QtQuick 1.0

Item {
    id: autoHide
    property bool shown: true
    property variant component: null

    Timer {
        id: autoHideTimer
        interval: 1000
        running: (component !== undefined) ? !component.containsMouse : false
        onTriggered: shown = false
    }

    Timer {
        id: edgeHitTimer
        interval: 500
        onTriggered: shown = true
    }

    Connections {
        target: component !== undefined ? component : null
        onOuterEdgeContainsMouseChanged: edgeHitTimer.running = outerEdgeContainsMouse
        ignoreUnknownSignals: true
    }
}
