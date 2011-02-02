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

    /* FIXME: it looks like Spread.qml is a useless extra layer whose content could fit
              very well in Workspace.qml */
    Windows {
        id: spread
        state: workspace.state == "screen" ? "screen" : "spread"
        anchors.fill: parent
    }

    MouseArea {
        anchors.fill: parent
        onClicked: workspace.clicked()
        enabled: workspace.state != "zoomed"
    }

    states: [
        State {
            name: "zoomed"
            PropertyChanges {
                target: workspace
                scale: zoomedScale
                x: zoomedX
                y: zoomedY
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
            NumberAnimation {
                target: workspace
                properties: "x,y,scale"
                duration: Utils.transitionDuration
                easing.type: Easing.InOutSine
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
