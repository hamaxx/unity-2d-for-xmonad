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
        if (state == "switcher") spread.activateSpread();
        else spread.cancelSpread();
    }

    transformOrigin: Item.TopLeft
    x: column * childrenRect.width
    y: row * childrenRect.height

    Spread {
        id: spread
        onExiting: switcher.state = ""
    }

    states: [
        State {
            name: "switcher"
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
                duration: 250
                easing.type: Easing.InOutSine
            }
        }
    ]
}
