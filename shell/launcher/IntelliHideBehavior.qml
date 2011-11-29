import QtQuick 1.0
import Unity2d 1.0

BaseBehavior {
    id: intellihide

    property bool behaviorShown: true
    property bool shown: (focusComponent && focusComponent.activeFocus) || behaviorShown

    Timer {
        id: edgeHitTimer
        interval: 500
        onTriggered: behaviorShown = true
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
        property: "behaviorShown"
        value: component ? component.containsMouse || !windows.intersects : true
    }
}
