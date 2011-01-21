import Qt 4.7
import UnityApplications 1.0
import UnityPlaces 1.0

Rectangle {
    id: switcher

    width: availableGeometry.width
    height: availableGeometry.height

    color: "black"

    property int workspaces: screen.workspaces
    property int columns: screen.columns
    property int rows: screen.rows

    /* These values are completely random. FIXME: pull from unity the proper ones */
    property int leftMargin: 40
    property int rightMargin: 40
    property int topMargin: 30
    property int spacing: 25

    /* Effective area available for laying out the workspaces after considering
       inter-workspace spaces */
    property int availableWidth: switcher.width - ((columns - 1) * spacing)
    property int availableHeight: switcher.height - ((rows - 1) * spacing)

    /* Scale of each workspace when laid out in the switcher grid
       Note that all scale calculations are done using the desktop's available
       geometry as this is the "natural" (initial) size of every workspace.

       FIXME: this seems to be broken in the case of 10 workspaces and 4x4 layout.
              it does only display a 3x3 grid for some reason.
    */
    property bool isLayoutHorizontal: (columns * availableGeometry.width) > (rows * availableGeometry.height)
    property real cellScale: (isLayoutHorizontal) ? (availableWidth / columns / switcher.width) :
                                                    (availableHeight / rows / switcher.height)

    /* Scale of a workspace when the user zooms on it (fills most of the switcher, leaving a margin to see
       the corners of the other workspaces below it) */
    property bool isDesktopHorizontal: availableGeometry.width > availableGeometry.height
    property real zoomedScale: (isDesktopHorizontal) ? ((width - leftMargin - rightMargin) / switcher.width) :
                                                       ((width - topMargin - bottomMargin) / switcher.height)

    property int transitionDuration: 250
    property variant zoomedWorkspace

    /* FIXME: this should be screen.currentWorkspace */
    property variant currentWorkspace: 0
    property string application

    Repeater {
        model: switcher.workspaces
        delegate: Workspace {
            id: workspace

            /* FIXME: This is ok right now since we ignore screen.orientation and
               screen.startingCorner, but we should respect them eventually */
            workspaceNumber: index
            row: Math.floor(index / columns)
            column: index % columns

            x: column * (switcher.width * cellScale) + (column * switcher.spacing)
            y: row * (switcher.height * cellScale) + (row * switcher.spacing)
            scale:  switcher.cellScale

            onActivated: exitTransitionTimer.start()
       }
    }

    property variant allWindows: globalWindowsList
    WindowsList {
        id: globalWindowsList
    }

    /* This controls the activation of the switcher */
    signal requestedToStart
    signal started
    Connections {
        target: control
        onActivateSpread: {
            application = switcher.allWindows.desktopFileForApplication(applicationId)

            // This gives a chance to any component to reset iself *before* being shown
            // FIXME: even though this should probably happen *after* we hide.
            requestedToStart()
            globalWindowsList.load()
            spreadView.show()
            spreadView.forceActivateWindow()
            started()
        }
    }

    /* This controls the deactivation of the switcher */
    function exitSwitcher()
    {
        if (zoomedWorkspace && zoomedWorkspace.selectedWindow) {
            /* If there's any selected window activate it, then clean up the selection */
            if (layout.selectedWindow) {
                zoomedWorkspace.selectedWindow.windowInfo.activate()
                zoomedWorkspace.selectedWindow = null
            }
        }
        zoomedWorkspace = null
        spreadView.hide()
    }

    Timer {
        id: exitTransitionTimer
        interval: transitionDuration
        onTriggered: exitSwitcher()
    }

    /* ---------------- FIXME: Anything below this line is here to allow debugging ----------------------- */
    property string oldApp
    Rectangle {
        id: debug1
        width: 150
        height: 30
        z: 500
        color: "green"

        Text {
            text: "Switch App/All"
        }

        MouseArea {
            anchors.fill:  parent
            onClicked: {
                var tmp = application
                application = oldApp
                oldApp = tmp
            }
        }
    }
    Rectangle {
        width: 150
        height: 30
        anchors.top: debug1.bottom
        z: 500
        color: "red"

        Text {
            text: "Hide!"
        }

        MouseArea {
            anchors.fill:  parent
            onClicked: spreadView.hide()
        }
    }
}

