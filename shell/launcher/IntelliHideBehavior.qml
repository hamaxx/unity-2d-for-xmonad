import QtQuick 1.0
import Unity2d 1.0
import "../common"
import "../common/utils.js" as Utils

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
        onContainsMouseChanged: {
            if (shown && target.containsMouse) {
                shownBecauseOfMousePosition = true
            }
            mouseLeaveTimer.running = !target.containsMouse
        }
        ignoreUnknownSignals: true
    }

    WindowsIntersectMonitor {
        id: windows
        monitoredArea: {
            if (intellihide.target) {
                if (Utils.isLeftToRight()) {
                    return Qt.rect(0,
                                   intellihide.target.y,
                                   intellihide.target.width,
                                   intellihide.target.height)
                } else {
                    return Qt.rect(screen.availableGeometry.width - intellihide.target.width,
                                   intellihide.target.y,
                                   intellihide.target.width,
                                   intellihide.target.height)
                }
            } else {
                return Qt.rect(0, 0, 0, 0)
            }
        }
    }
}
