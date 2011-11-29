import QtQuick 1.0
import Unity2d 1.0

BaseBehavior {
    id: autoHide
    property bool behaviorShown: true
    property bool shown: (focusComponent && focusComponent.activeFocus) || behaviorShown

    Timer {
        id: autoHideTimer
        interval: 1000
        running: (component !== undefined) ? !component.containsMouse : false
        onTriggered: behaviorShown = false
    }

    Timer {
        id: edgeHitTimer
        interval: 500
        onTriggered: behaviorShown = true
    }

    Connections {
        target: component !== undefined ? component : null
        onOuterEdgeContainsMouseChanged: edgeHitTimer.running = outerEdgeContainsMouse
        ignoreUnknownSignals: true
    }
}
