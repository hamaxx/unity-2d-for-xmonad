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

    /* FIXME: this shouldn't be kept here. Remove when the DBUS API settles */
    property string application

    /* The number of the currently zoomed workspace. Use workspaceForNumber to find the
       actual Workspace component. -1 Means that no workspace is zoomed */
    property int zoomedWorkspace: -1

    /* This is the master model containing all the windows recognized by bamf for the entire
       screen. Each workspace will individually filter them to select only those that
       belong to it. */
    property variant allWindows: WindowsList { }

    /* Those signals are emitted by the swicher while it starts (beforeShowing
       and afterShowing) and while it exits (beforeHiding and afterHiding). The single
       workspaces react to these signals and perform the appropriate setup and cleanup
       operations */
    signal beforeShowing
    signal afterShowing
    signal beforeHiding
    signal afterHiding

    /* Group all Workspace elements into a single focus scope, so that only one of them
       has keyboard focus at the same time. This grouping also helps workspaceByNumber
       iterate over less items than it would need to if the Repeater was adding children
       to the switcher itself. */
    FocusScope {
        id: spaces
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

                /* We need to let the transition animation finish entirely before hiding
                   the window and performing cleanup operations */
                onActivated: exitTransitionTimer.start()
            }
        }
    }

    /* We want to avoid storing references to Workspace objects directly in
       properties of the switcher. This is because items may disappear at any
       time from the underlying model, making these refereces unreliable.
       Therefore we just go and find them by their workspace number among the
       children list on demand. */
    function workspaceByNumber(number) {
        if (number != null) {
            for (var i = 0; i < spaces.children.length; i++) {
                var child = spaces.children[i];
                if (child.workspaceNumber == number)
                    return child;
            }
        }
        return null
    }

    /* This controls the activation of the switcher via DBUS */
    /* FIXME: there should be more DBUS messages (names are not final):
       - SpreadAllWindows: workspace switcher of all windows
       - SpreadWorkspace: same as above but with 1 workspace already zoomed
       - SpreadApplicationWindows: like SpreadAllWindows but for only 1 app
       - ActivateSpread: like SpreadWorkspace but for 1 app and with no other
                         workspaces in the background (like the old one.
                         Requested by Bill)
    */
    Connections {
        target: control

        onActivateSpread: {
            /* FIXME: desktopFileForApplication should be moved to ScreenInfo */
            singleApplication = switcher.allWindows.desktopFileForApplication(applicationId)

            beforeShowing()
            allWindows.load()

            currentTransitionDuration = transitionDuration
            spreadView.show()
            spreadView.forceActivateWindow()
            afterShowing()
        }
    }

    /* This controls the exit from the switcher.
       Note that we can't just hide the workspaces switcher immediately when the user
       wants to activate a window or cancel the switching process. We first want any
       transitions to complete, so we need to start this timer, and when it's triggered
       it will actually do all that is necessary to hide the switcher and cleanup */
    Timer {
        id: exitTransitionTimer
        interval: transitionDuration
        onTriggered: {
            beforeHiding()

            spreadView.hide()
            currentTransitionDuration = 0

            afterHiding()

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
        /* If the currently active workspace is also the zoomed one, don't
           bother unzooming it */
        if (zoomedWorkspace != -1 && zoomedWorkspace != currentWorkspace) {

            var zoomed = switcher.workspaceByNumber(switcher.zoomedWorkspace)
            if (zoomed) zoomed.unzoom()
        }

        var current = switcher.workspaceByNumber(switcher.currentWorkspace)
        if (current) {
            current.selectedXid = 0
            current.activate()
        }

        /* Let the transition finish and the hide the switcher and perform
           cleanup */
        exitTransitionTimer.start()
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
