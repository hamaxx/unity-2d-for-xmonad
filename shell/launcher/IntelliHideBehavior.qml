import QtQuick 1.0
import Unity2d 1.0

BaseBehavior {
    id: intellihide

    property bool shownRegardlessOfFocus: true
    property bool shown: (monitorFocusOn && monitorFocusOn.activeFocus) || shownRegardlessOfFocus

    Timer {
        id: edgeHitTimer
        interval: 500
        onTriggered: shownRegardlessOfFocus = true
    }

    Connections {
        target: (intellihide.target !== undefined) ? intellihide.target : null
        onOuterEdgeContainsMouseChanged: edgeHitTimer.running = outerEdgeContainsMouse
        ignoreUnknownSignals: true
    }

    WindowsIntersectMonitor {
        id: windows
        monitoredArea: intellihide.target ? Qt.rect(0, intellihide.target.y,
                                                    intellihide.target.width,
                                                    intellihide.target.height)
                                          : Qt.rect(0, 0, 0, 0)
    }

    /* For some reason this requires a Binding, assigning directly to the property
       won't work in certain cases. */
    Binding {
        target: intellihide
        property: "shownRegardlessOfFocus"
        value: intellihide.target ? intellihide.target.containsMouse || !windows.intersects : true
    }
}
