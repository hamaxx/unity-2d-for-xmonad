import QtQuick 1.0
import Unity2d 1.0
import "../common"
import "../common/utils.js" as Utils

Loader {
    id: launcherLoader
    source: "Launcher.qml"
    property variant visibilityController: visibilityController
    onLoaded: item.focus = true

    VisibilityController {
        id: visibilityController
        behavior: launcherBehavior.status == Loader.Ready ? launcherBehavior.item : null
    }

    Loader {
        id: launcherBehavior

        property variant modesMap: { 0: '../common/AlwaysVisibleBehavior.qml',
                                     1: 'AutoHideBehavior.qml',
                                     2: 'IntelliHideBehavior.qml' }

        source: modesMap[Utils.clamp(launcher2dConfiguration.hideMode, 0, 2)]
    }

    StrutManager {
        id: strutManager
        edge: Unity2dPanel.LeftEdge
        widget: declarativeView
        height: item.height
        width: item.width
        enabled: Utils.clamp(launcher2dConfiguration.hideMode, 0, 2) == 0

        Component.onCompleted: {
            strutManager.updateStrut()
        }
    }

    Binding {
        target: launcherBehavior
        property: "item.target"
        value: launcherLoader.item
        when: launcherBehavior.status == Loader.Ready
    }

    Binding {
        target: launcherBehavior.item
        property: "forcedVisible"
        value: visibilityController.forceVisible
    }

    Connections {
        target: declarativeView
        onSuperKeyHeldChanged: {
            if (superKeyHeld) visibilityController.beginForceVisible()
            else visibilityController.endForceVisible()
        }
    }

    Binding {
        target: launcherLoader.item
        property: "outerEdgeContainsMouse"
        value: outerEdge.containsMouse && outerEdge.enabled
    }

    MouseArea {
        id: outerEdge
        anchors.fill: parent
        anchors.margins: -1
        hoverEnabled: !visibilityController.shown
        enabled: !visibilityController.shown
    }

}
