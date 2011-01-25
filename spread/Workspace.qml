import Qt 4.7

FocusScope {
    id: workspace

    transformOrigin: Item.TopLeft
    z: 1

    property int row
    property int column
    property int workspaceNumber
    property alias selectedXid: spread.selectedXid
    property bool isZoomed: switcher.workspaceByNumber(switcher.zoomedWorkspace) == workspace

    /* Give keyboard focus (among all workspaces) only to the zoomed Workspace.
       Since the workspace is a FocusScope, only one Item inside it can have focus.
       In our case it's always the KeyboardNavigator inside the SpreadLayout. */
    focus: isZoomed

    /* This is emitted, by the zoomed workspace only, whenever a window or the workspace
       background is clicked or ENTER is pressed while a window is selected */
    signal activated

    function activate() {
        workspace.state = "screen"
        spread.state = ""
        activated()
    }

    function unzoom() {
        spread.selectedXid = 0
        workspace.state = ""
        switcher.zoomedWorkspace = -1
    }

    function zoom() {
        workspace.state = "zoomed"
        switcher.zoomedWorkspace = workspace.workspaceNumber
    }

    /* We listen to the switcher's signals during its startup and exit phases
       to setup and cleanup the state of this specific workspace, so that it
       will interact correctly each time the switcher is summoned.
       The currently active workspace shoud start maximized, then transition
       to zoomed state. All the others should remain in the background. */
    Connections {
        target: switcher
        onBeforeShowing: {
            if (workspaceNumber == switcher.currentWorkspace) {
                switcher.zoomedWorkspace = workspace.workspaceNumber
                workspace.state = "screen"
            }
        }

        onAfterShowing: {
            if (isZoomed) workspace.state = "zoomed"
            spread.state = "spread"
        }

        onBeforeHiding: {
            if (isZoomed) screen.activateWorkspace(workspace.workspaceNumber)
        }

        onAfterHiding: {
            /* FIXME: this should be done before hiding. However for some reason
               it always fail for windows on the current workspace. Should be put
               back in onBeforeHiding though, since it does cause a minor visual
               glitch if done here. */
            if (isZoomed) activateSelectedWindow(true)

            workspace.state = ""
            spread.state = ""
            z = 1
        }
    }

    Spread {
        id: spread
        application: switcher.application

        /* When a window or the spread background is clicked or a window is
           activated by keyboard interact in the following way:
           - If another workspace is zoomed, cancel the current zoom.
           - If we are zooomed, then maximize our workspace and trigger an exit.
           - If no workspace is zoomed, then make us the zoomed workspace. */
        onWindowActivated: {
            if (switcher.zoomedWorkspace != -1) {
                var zoomed = switcher.workspaceByNumber(switcher.zoomedWorkspace)
                if (zoomed) {
                    if (zoomed != workspace) zoomed.unzoom()
                    else zoomed.activate()
                }
            } else if (workspace.state == "") zoom()
        }
    }

    /* When this is called whatever window was selected (by keyboard or mouse) on
       this workspace, if any, will be made the active window. As a convenience,
       we allow to clean the selection afterwards. */
    function activateSelectedWindow(clearSelection) {
        if (spread.selectedXid != 0) {
            var selectedWindow = spread.windowForXid(spread.selectedXid)
            if (selectedWindow != null) selectedWindow.windowInfo.activate()
            if (clearSelection) spread.selectedXid = 0;
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
            /* We want the value of z to be set to 2 immediately, and during the entire animation,
               regardless of the final state it will have */
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
