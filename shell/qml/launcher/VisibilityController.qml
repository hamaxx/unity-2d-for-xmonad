import QtQuick 1.0

Item {
    id: controller
    property bool shown: true
    property variant launcher: null

    Loader {
        id: visibilityBehavior
        source: "AutoHideBehavior.qml"
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
