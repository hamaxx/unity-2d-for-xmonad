import Qt 4.7
import Unity2d 1.0
import "utils.js" as Utils

Rectangle {
    id: switcher

    /* FIXME: setting width and height is not useful since we use
       view.setResizeMode(QDeclarativeView::SizeRootObjectToView)
    */
    width: screen.availableGeometry.width
    height: screen.availableGeometry.height

    color: "black"

    property int columns: screen.workspaces.columns
    property int rows: screen.workspaces.rows

    /* FIXME: These values are completely random. Pull from unity the proper ones */
    property int margin: 40
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
    property bool isLayoutHorizontal: (columns * screen.availableGeometry.width) >
                                      (rows * screen.availableGeometry.height)
    property real cellScale: (isLayoutHorizontal) ? (availableWidth / columns / switcher.width) :
                                                    (availableHeight / rows / switcher.height)

    /* Scale of a workspace when the user zooms on it (fills most of the switcher, leaving a margin to see
       the corners of the other workspaces below it) */
    property bool isDesktopHorizontal: screen.availableGeometry.width > screen.availableGeometry.height
    property real zoomedScale: (isDesktopHorizontal) ? ((width - 2*margin) / switcher.width) :
                                                       ((width - 2*margin) / switcher.height)

    /* When this is set, it is used to filter the global list of windows to limit it to
       a single application. See the QSortFilterProxyModelQML used in Spread.qml */
    property string applicationFilter

    /* The number of the currently zoomed workspace (-1 means no workspace)
       Use workspaceForNumber to find the actual Workspace component. */
    property int zoomedWorkspace: -1

    /* This is the master model containing all the windows recognized by bamf for the entire
       screen. Each workspace will individually filter them to select only those that
       belong to it. */
    property variant allWindows: WindowsList { }
    property int lastActiveWindow: 0

    /* Group all Workspace elements into a single Item to help workspaceByNumber
       iterate over less items than it would need to if the Repeater was adding children
       to the switcher itself. */
    Item {
        id: spaces
        Repeater {
            model: screen.workspaces.count
            delegate: Workspace {
                id: workspace

                /* FIXME: This is ok right now since we ignore screen.orientation and
                   screen.startingCorner, but we should respect them eventually */
                property int workspaceNumber: index
                property int row: Math.floor(index / columns)
                property int column: index % columns

                x: column * (switcher.width * cellScale) + (column * switcher.spacing)
                y: row * (switcher.height * cellScale) + (row * switcher.spacing)
                width: switcher.width
                height: switcher.height
                scale:  switcher.cellScale

                zoomedX: switcher.margin
                zoomedY: switcher.margin
                zoomedScale: switcher.zoomedScale

                focus: zoomedWorkspace == workspaceNumber

                state: {
                    if (initial) {
                        if (screen.workspaces.current == workspaceNumber) {
                            return "screen"
                        } else {
                            return ""
                        }
                    } else {
                        if (zoomedWorkspace == workspaceNumber) {
                            return "zoomed"
                        } else {
                            return ""
                        }
                    }
                }

                onClicked: if (zoomedWorkspace != -1) {
                               zoomedWorkspace = -1
                           } else {
                               zoomedWorkspace = workspaceNumber
                           }
            }
        }
    }

    property bool initial: true

    /* This connection receives all commands from the DBUS API */
    Connections {
        target: control

        onShowCurrentWorkspace: {
            /* Setup application pre-filtering and initially zoomed desktop, if any
               were specified as arguments */
            applicationFilter = applicationDesktopFile
            zoomedWorkspace = screen.workspaces.current
            show()
        }

        onShowAllWorkspaces: {
            applicationFilter = applicationDesktopFile
            zoomedWorkspace = -1
            show()
        }

        onHide: cancelAndExit()

        onFilterByApplication: applicationFilter = applicationDesktopFile
    }

    function show() {
        /* Save the currently active window before showing and activating the switcher,
           so that we can use it to pre-select the active window on the workspace */
        lastActiveWindow = screen.activeWindow

        allWindows.load()

        spreadView.show()
        spreadView.forceActivateWindow()
        initial = false
    }

    /* This controls the exit from the switcher.
       Note that we can't just hide the workspaces switcher immediately when the user
       wants to activate a window or cancel the switching process. We first want any
       transitions to complete, so we need to start this timer, and when it's triggered
       it will actually do all that is necessary to hide the switcher and cleanup */
    Timer {
        id: exitTransitionTimer
        interval: Utils.transitionDuration
        onTriggered: {
            spreadView.hide()

            /* Nothing should be allowed to touch the windows anymore here, so it should
               be safe to unload them all to save memory.
               NOTE: i'm not exactly sure any memory will actually be saved since the biggest
               usage is the window screenshots, and Qt is caching them (see SpreadWindow.qml
               for the trick I use to force them to refresh and more info on this cache)
            */
            allWindows.unload()
            zoomedWorkspace = -1
        }
    }


    /* Handle both the ESC keypress and any click on the area outside of the
       switcher in the same way: maximize the currently active workspace to screen
       size and hide the switcher (effectively canceling the switching operation).
       If another workspace was zoomed unzoom it first. */
    Keys.onPressed: {
        switch (event.key) {
        case Qt.Key_Escape:
            cancelAndExit()
            event.accepted = true
            return
        }
    }

    Connections {
        target: spreadView
        onOutsideClick: cancelAndExit()
    }

    function cancelAndExit() {
        /* Expand back to screen size the current workspace */
        zoomedWorkspace = screen.workspaces.current
        initial = true

        /* Let the transition finish and then hide the switcher and perform cleanup */
        exitTransitionTimer.start()
    }

    function activateWindow(windowInfo) {
        if (windowInfo.workspace != zoomedWorkspace) {
            zoomedWorkspace = windowInfo.workspace
        } else {
            screen.workspaces.current = zoomedWorkspace
            windowInfo.activate()
            cancelAndExit()
        }
    }
}
