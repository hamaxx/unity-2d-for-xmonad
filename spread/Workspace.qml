import Qt 4.7
import "utils.js" as Utils

FocusScope {
    id: workspace

    transformOrigin: Item.TopLeft /* FIXME: useful? */

    property bool isZoomed: switcher.workspaceByNumber(switcher.zoomedWorkspace) == workspace
    signal clicked

    GnomeBackground {
        anchors.fill: parent
        overlay_color: "black"
        overlay_alpha: 0
    }

    MouseArea {
        z: 1 /* Above Spread */
        anchors.fill: parent
        onClicked: workspace.clicked()
    }

    /* We listen to the switcher's signals during its startup and exit phases
       to setup and cleanup the state of this specific workspace, so that it
       will interact correctly each time the switcher is summoned.
       The currently active workspace shoud start maximized, then transition
       to zoomed state. All the others should remain in the background. */
    Connections {
        target: switcher
        onBeforeShowing: {
            if (isZoomed) {
                spread.state = "screen"
            } else {
                spread.state = "spread"
            }
        }

        onAfterShowing: {
            if (isZoomed) {
                spread.state = "spread"
            }
        }
    }

    Spread {
        id: spread
    }

    /* When this is called whatever window was selected (by keyboard or mouse) on
       this workspace, if any, will be made the active window. As a convenience,
       we allow to clean the selection afterwards. */
    /*function activateSelectedWindow(clearSelection) {
        if (spread.selectedXid != 0) {
            var selectedWindow = spread.windowForXid(spread.selectedXid)
            if (selectedWindow != null) selectedWindow.windowInfo.activate()
//            if (clearSelection) spread.selectedXid = 0;
        }
    }*/

    states: [
        State {
            name: "zoomed"
            PropertyChanges {
                target: workspace
                scale: switcher.zoomedScale
                x: switcher.leftMargin
                y: switcher.topMargin
                z: 1
            }
        },
        State {
            name: "screen"
            PropertyChanges {
                target: workspace
                scale: 1.0
                x: 0
                y: 0
                z: 1
            }
        }
    ]

    transitions: [
        Transition {
            SequentialAnimation {
                NumberAnimation {
                    target: workspace
                    properties: "x,y,scale"
                    duration: Utils.transitionDuration
                    easing.type: Easing.InOutSine
                }
            }
        },
        Transition {
            from: "zoomed"
            PropertyAction { property: "z"; value: 2 }
            NumberAnimation {
                target: workspace
                properties: "x,y,scale"
                duration: Utils.transitionDuration
                easing.type: Easing.InOutSine
            }
        }
    ]
}
