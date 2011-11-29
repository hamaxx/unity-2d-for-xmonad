import QtQuick 1.0
import Unity2d 1.0

Item {
    id: intellihide

    property variant component: null
    property bool shown: true

    Timer {
        id: edgeHitTimer
        interval: 500
        onTriggered: shown = true
    }

    Connections {
        target: (component !== undefined) ? component : null
        onOuterEdgeContainsMouseChanged: edgeHitTimer.running = outerEdgeContainsMouse
        ignoreUnknownSignals: true
    }

    WindowsIntersectMonitor {
        id: windows
        monitoredArea: component ? Qt.rect(0, component.y, component.width, component.height)
                                : Qt.rect(0, 0, 0, 0)
    }

    /* For some reason this requires a Binding, assigning directly to the property
       won't work in certain cases. */
    Binding {
        target: intellihide
        property: "shown"
        value: component ? component.containsMouse || !windows.intersects : true
    }
}
