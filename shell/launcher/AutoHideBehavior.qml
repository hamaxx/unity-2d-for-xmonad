import QtQuick 1.0
import Unity2d 1.0
import "../common"

BaseBehavior {
    id: autoHide
    property bool shownRegardlessOfFocus: true

    shown: target !== undefined && (target.activeFocus || shownRegardlessOfFocus)

    Timer {
        id: autoHideTimer
        property bool oneTimeTrigger: false

        interval: 1000
        running: oneTimeTrigger || ((target !== undefined) ? !target.containsMouse : false)
        onTriggered: {
            oneTimeTrigger = false
            shownRegardlessOfFocus = false
        }
    }

    Timer {
        id: edgeHitTimer
        interval: 500
        onTriggered: shownRegardlessOfFocus = true
    }

    ShowDesktopMonitor {
        onShownChanged: {
            autoHideTimer.oneTimeTrigger = shown
            shownRegardlessOfFocus = shown
        }
    }

    Connections {
        target: autoHide.target !== undefined ? autoHide.target : null
        onOuterEdgeContainsMouseChanged: edgeHitTimer.running = target.outerEdgeContainsMouse
        ignoreUnknownSignals: true
    }
}
