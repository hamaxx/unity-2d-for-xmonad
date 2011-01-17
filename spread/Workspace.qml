import Qt 4.7

Item {
    id: workspace

    property alias applicationId: spread.applicationId
    property int row
    property int column

    property real cellScale
    property int cellX
    property int cellY

    onStateChanged: {
        console.log("STATE changed to " + state + ".")
        if (state == "switching") spread.activateSpread();
        else spread.cancelSpread();
    }

    transformOrigin: Item.TopLeft
    x: column * childrenRect.width
    y: row * childrenRect.height

    Spread {
        id: spread

        onExitSwitch: {
            console.log("EXITING EXITING")
            workspace.state = ""
        }
    }

    Component.onCompleted: {
        console.log("x, y " + x + ", " + y)
    }

    states: [
        State {
            name: "switching"
            PropertyChanges {
                target: workspace
                scale: cellScale
                x: cellX
                y: cellY
            }
        }
    ]

    transitions: [
        Transition {
            NumberAnimation {
                target: workspace
                properties: "x,y,scale"
                duration: 4000
                easing.type: Easing.InOutSine
            }
        }
    ]
}
