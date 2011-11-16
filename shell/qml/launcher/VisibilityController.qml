import QtQuick 1.0
import QConf 1.0

Item {
    id: controller
    property bool shown: true
    property variant launcher: null

    QConf {
        id: configuration
        schema: "com.canonical.Unity2d.Launcher"
    }

    Loader {
        id: visibilityBehavior
        source: {
            var modesMap = { 0: 'AlwaysVisible', 1: 'AutoHide', 2: 'IntelliHide' }
            return modesMap[configuration.hideMode] + "Behavior.qml"
        }
    }

    Binding {
        target: visibilityBehavior
        property: "item.launcher"
        value: launcher
        when: visibilityBehavior.progress == 1.0
    }

    Binding {
        target: controller
        property: "shown"
        value: visibilityBehavior.item.shown
    }
}
