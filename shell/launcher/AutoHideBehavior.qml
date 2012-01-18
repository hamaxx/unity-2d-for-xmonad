import QtQuick 1.0
import Unity2d 1.0
import "../common"

BaseBehavior {
    id: autoHide
    property bool shownRegardlessOfFocus: false

    shown: target !== undefined && (target.activeFocus || shownRegardlessOfFocus)

    Timer {
        id: autoHideTimer

        interval: 1000
        onTriggered: shownRegardlessOfFocus = false
    }

    Timer {
        id: edgeHitTimer
        interval: 500
        onTriggered: shownRegardlessOfFocus = true
    }

    ShowDesktopMonitor {
        onShownChanged: {
            if (shown) {
                shownRegardlessOfFocus = true
                autoHideTimer.restart()
            }
        }
    }

    Connections {
        target: autoHide.target !== undefined ? autoHide.target : null
        onOuterEdgeContainsMouseChanged: edgeHitTimer.running = target.outerEdgeContainsMouse
        ignoreUnknownSignals: true
    }

    Connections {
        target: autoHide.target !== undefined ? autoHide.target : null
        onContainsMouseChanged: autoHideTimer.running = !target.containsMouse
        ignoreUnknownSignals: true
    }
}
