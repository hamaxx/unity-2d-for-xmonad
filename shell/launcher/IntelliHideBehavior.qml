import QtQuick 1.0
import Unity2d 1.0
import "../common"

BaseBehavior {
    id: intellihide

    property bool shownBecauseOfMousePosition: false

    shown: target !== undefined && (target.activeFocus || shownBecauseOfMousePosition || !windows.intersects)

    onForcedVisibleChanged:
    {
        if (!forcedVisible) {
            if (!target.containsMouse) {
                shownBecauseOfMousePosition = true
                mouseLeaveTimer.restart()
            }
        }
    }

    Timer {
        id: edgeHitTimer
        interval: 500
        onTriggered: shownBecauseOfMousePosition = true
    }

    Timer {
        id: mouseLeaveTimer
        interval: 1000
        onTriggered: shownBecauseOfMousePosition = false
    }

    Connections {
        target: (intellihide.target !== undefined) ? intellihide.target : null
        onOuterEdgeContainsMouseChanged: edgeHitTimer.running = target.outerEdgeContainsMouse
        ignoreUnknownSignals: true
    }

    Connections {
        target: (intellihide.target !== undefined) ? intellihide.target : null
        onContainsMouseChanged: mouseLeaveTimer.running = !target.containsMouse
        ignoreUnknownSignals: true
    }

    WindowsIntersectMonitor {
        id: windows
        monitoredArea: intellihide.target ? Qt.rect(0, intellihide.target.y,
                                                    intellihide.target.width,
                                                    intellihide.target.height)
                                          : Qt.rect(0, 0, 0, 0)
    }
}
