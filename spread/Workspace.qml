import Qt 4.7

Item {
    id: workspace
    z: 1

    property int row
    property int column

    property int workspaceNumber
    property alias selectedWindow: spread.selectedWindow

    signal activated

    Connections {
        target: switcher
        onBeforeShowing: {
            if (workspaceNumber == switcher.currentWorkspace) workspace.state = "screen"
        }

        onAfterShowing: {
            if (workspaceNumber == switcher.currentWorkspace) {
                switcher.zoomedWorkspace = workspace
                workspace.state = "zoomed"
            }
            spread.state = "spread"
        }

        onAfterHiding: {
            workspace.state = ""
            spread.state = ""
        }
    }

    transformOrigin: Item.TopLeft
    x: column * childrenRect.width
    y: row * childrenRect.height

    /* If another workspace is zoomed, cancel the current zoom.
       If we are zooomed, then exit to our workspace (optionally
       to the selected window if any) */
    function handleSpreadClick(on_window)
    {
        if (switcher.zoomedWorkspace) {
            if (switcher.zoomedWorkspace != workspace)
                switcher.zoomedWorkspace.state = ""
            else {
                /* We are exiting the switcher */
                workspace.state = "screen"
                spread.state = ""
                activated()
            }
            switcher.zoomedWorkspace = null
        } else if (workspace.state == "") {
            /* We are zooming this workspace */
            workspace.state = "zoomed"
            switcher.zoomedWorkspace = workspace
        }
    }

    Spread {
        id: spread
        application: switcher.application
        workspace: workspaceNumber

        onBackgroundClicked: handleSpreadClick(false)
        onWindowClicked: handleSpreadClick(true)
    }

    Connections {
        target: spreadView
        onOutsideClick: {
            selectedWindow = null
            if (workspaceNumber == switcher.currentWorkspace) {
                /* Go immediately to screen state regardless
                   of previous states if we are the current workspace */
                spread.state = ""
                workspace.state = "screen"
                activated()
            }
        }
    }

    states: [
        State {
            name: "zoomed"
            PropertyChanges {
                target: workspace
                scale: switcher.zoomedScale
                z: 2
                x: switcher.leftMargin
                y: switcher.topMargin
            }
        },
        State {
            name: "screen"
            PropertyChanges {
                target: workspace
                scale: 1.0
                z: 2
                x: 0
                y: 0
            }
        }
    ]

    transitions: [
        Transition {
            SequentialAnimation {
                PropertyAction { target: workspace; property: "z"; value: 2 }
                NumberAnimation {
                    target: workspace
                    properties: "x,y,scale"
                    duration: switcher.currentTransitionDuration
                    easing.type: Easing.InOutSine
                }
                PropertyAction { target: workspace; property: "z"; value: 2 }
            }
        }
    ]
}
