import QtQuick 1.0
import "../common"

BaseBehavior {
    id: autoHide
    property bool shownRegardlessOfFocus: true

    shown: target !== undefined && (target.activeFocus || shownRegardlessOfFocus)

    Timer {
        id: autoHideTimer
        interval: 1000
        running: (target !== undefined) ? !target.containsMouse : false
        onTriggered: shownRegardlessOfFocus = false
    }

    Timer {
        id: edgeHitTimer
        interval: 500
        onTriggered: shownRegardlessOfFocus = true
    }

    Connections {
        target: autoHide.target !== undefined ? autoHide.target : null
        onOuterEdgeContainsMouseChanged: edgeHitTimer.running = target.outerEdgeContainsMouse
        ignoreUnknownSignals: true
    }
}
