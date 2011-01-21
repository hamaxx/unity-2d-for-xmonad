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
    property variant currentWorkspace: screen.currentWorkspace

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

    /* We don't really want to animate anything that happens while the window isn't visible,
       so we set the transition duration for all animations to zero unless we're visible */
    property int transitionDuration: 250
    property int currentTransitionDuration: 0
    property variant zoomedWorkspace

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

    signal beforeShowing
    signal afterShowing
    signal afterHiding
    Connections {
        target: control

        /* This controls the activation of the switcher */
        /* FIXME: there should be more DBUS messages (names are not final):
           - SpreadAllWindows: workspace switcher of all windows
           - SpreadWorkspace: same as above but with 1 workspace already zoomed
           - SpreadApplicationWindows: like SpreadAllWindows but for only 1 app
           - ActivateSpread: like SpreadWorkspace but for 1 app and with no other
                             workspaces in the background (like the old one.
                             Requested by Bill)
        */
        onActivateSpread: {
            singleApplication = switcher.allWindows.desktopFileForApplication(applicationId)

            /* FIXME: all code below should be refactored in function once the other DBUS
                      methods are added. */
            beforeShowing()
            globalWindowsList.load()

            currentTransitionDuration = transitionDuration
            spreadView.show()
            spreadView.forceActivateWindow()
            afterShowing()
        }
    }

    /* This controls the deactivation of the switcher */
    function exitSwitcher()
    {
        if (zoomedWorkspace && zoomedWorkspace.selectedWindow) {
            /* If there's any selected window activate it, then clean up the selection */
            if (layout.selectedWindow) {
                console.log("WE ARE SELECTING A WINDOW")
                zoomedWorkspace.selectedWindow.windowInfo.activate()
                zoomedWorkspace.selectedWindow = null
            }
        }
        zoomedWorkspace = null

        spreadView.hide()
        currentTransitionDuration = 0
        afterHiding()

        /* Nothing should be allowed to touch the windows anymore here, so it should
           be safe to unload them to save memory.
           NOTE: i'm not exactly sure any memory will actually be saved since the biggest
           usage is the window screenshots, and Qt is caching them (see SpreadWindow.qml
           for the trick I use to force them to refresh and more info on this cache)
        */
        globalWindowsList.unload()
    }

    Timer {
        id: exitTransitionTimer
        interval: transitionDuration
        onTriggered: exitSwitcher()
    }

    /* ---------------- FIXME: Anything below this line is here to allow debugging ----------------------- */

    property string singleApplication
    Rectangle {
        id: debug1
        width: text1.paintedWidth
        height: text1.paintedHeight
        anchors.bottom: debug2.top
        border.color: "yellow"
        border.width: 1

        z: 500
        color: "white"

        Text {
            id: text1
            text: "Only " + singleApplication
        }

        MouseArea {
            anchors.fill:  parent
            onClicked: application = singleApplication
        }
    }
    Rectangle {
        id: debug2
        z: 500
        width: text2.paintedWidth
        height: text2.paintedHeight
        color: "white"
        anchors.bottom: switcher.bottom
        border.color: "yellow"
        border.width: 1


        Text {
            id: text2
            text: "All windows"
        }

        MouseArea {
            anchors.fill:  parent
            onClicked: application = ""
        }
    }
}

