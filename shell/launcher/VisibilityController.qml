import QtQuick 1.0
import QConf 1.0
import "../common/utils.js" as Utils

Item {
    id: controller
    property bool shown: true
    property variant launcher: null
    property variant modesMap: { 0: 'AlwaysVisibleBehavior.qml',
                                 1: 'AutoHideBehavior.qml',
                                 2: 'IntelliHideBehavior.qml' }
    property bool forceVisible: false
    property variant forceVisibleStack: {}

    Loader {
        id: visibilityBehavior
        source: modesMap[(forceVisible) ? 0 : Utils.clamp(launcher2dConfiguration.hideMode, 0, 2)]
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

    function beginForceVisible(id) {
        /* We need to copy the property to a JS variable before we can properly interact
           with it and then copy it back to the QML property when done. This seems to be
           a limitation of QML that will be fixed in QtQuick 2.0. For more information:
           https://bugreports.qt.nokia.com/browse/QTBUG-21842
        */
        var stack = controller.forceVisibleStack
        var wasEmpty = Utils.hashEmpty(stack)

        if (stack[id]) stack[id] += 1
        else stack[id] = 1
        controller.forceVisibleStack = stack

        if (wasEmpty) forceVisible = true
    }

    function endForceVisible(id) {
        var stack = controller.forceVisibleStack

        if (stack[id]) {
            stack[id] -= 1
            if (stack[id] === 0) delete stack[id]
        } else console.log("WARNING: endForceVisible for id \"" + id +
                           "\" called without matching startForceVisible")

        controller.forceVisibleStack = stack
        if (Utils.hashEmpty(stack)) forceVisible = false
    }
}
