import Qt 4.7
import "utils.js" as Utils

FocusScope {
    id: workspace

    transformOrigin: Item.TopLeft /* FIXME: useful? */

    property real zoomedScale
    property int zoomedX
    property int zoomedY

    signal clicked

    GnomeBackground {
        anchors.fill: parent
        overlay_color: "black"
        overlay_alpha: 0
    }

    Windows {
        state: workspace.state == "screen" ? "screen" : "spread"
        anchors.fill: parent
        focus: true
        onClicked: workspace.clicked()
        onWindowActivated: {
            if (workspace.state != "zoomed") {
                workspace.clicked()
            } else {
                /* Hack to make sure the window is on top of the others during the
                   outro animation */
                window.z = 9999
                switcher.activateWindow(window.windowInfo)
            }
        }
    }

    states: [
        State {
            name: "zoomed"
            PropertyChanges {
                target: workspace
                scale: zoomedScale
                x: zoomedX
                y: zoomedY
                z: 2
            }
        },
        State {
            name: "screen"
            PropertyChanges {
                target: workspace
                scale: 1.0
                x: 0
                y: 0
                z: 2
            }
        }
    ]

    transitions: [
        Transition {
            NumberAnimation {
                target: workspace
                properties: "x,y,scale"
                duration: Utils.transitionDuration
                easing.type: Easing.InOutSine
            }
        },
        Transition {
            to: ""
            SequentialAnimation {
                /* When going to default state put the workspace underneath the
                   workspace in zoomed state but not on the same plane as the
                   workspaces also in the default state until the end of the transition. */
                PropertyAction { property: "z"; value: 1 }
                NumberAnimation {
                    target: workspace
                    properties: "x,y,scale"
                    duration: Utils.transitionDuration
                    easing.type: Easing.InOutSine
                }
                PropertyAction { property: "z"; value: 0 }
            }
        }
    ]
}
