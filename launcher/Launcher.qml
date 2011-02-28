import Qt 4.7
import UnityApplications 1.0
import Unity2d 1.0 /* required for drag’n’drop handling */

Item {
    id: launcher

    Image {
        id: background

        anchors.fill: parent
        fillMode: Image.TileVertically
        source: "artwork/background.png"
    }

    LauncherList {
        id: main
        anchors.top: parent.top
        anchors.bottom: shelf.top
        width: parent.width
        z: 1 /* for dnd to remain on top of looseItems */

        paddingTop: 5
        paddingBottom: 5
        autoScrollSize: tileSize / 2
        autoScrollVelocity: 200

        model: ListAggregatorModel {
            id: items
        }

        // FIXME: dragging the list to flick it exhibits unpleasant visual
        // artifacts, and its contentY sometimes remains blocked at a position
        // too far off the boundaries of the list.
        MouseArea {
            /* Handle drag’n’drop to re-order applications. */
            id: dnd
            anchors.fill: parent

            /* id (desktop file path) of the application being dragged */
            property string currentId: ""
            /* list index of the application being dragged */
            property int currentIndex
            /* absolute mouse coordinates in the list */
            property variant listCoordinates: mapToItem(main.contentItem, mouseX, mouseY)
            /* list index of the application underneath the cursor */
            property int index: main.indexAt(listCoordinates.x, listCoordinates.y)

            Timer {
                id: longPressDelay
                /* The standard threshold for long presses is hard-coded to 800ms
                   (http://doc.qt.nokia.com/qml-mousearea.html#onPressAndHold-signal).
                   This value is too high for our use case. */
                interval: 500 /* in milliseconds */
                onTriggered: {
                    if (main.moving) return
                    dnd.parent.interactive = false
                    var id = items.get(dnd.currentIndex).desktop_file
                    if (id != undefined) dnd.currentId = id
                }
            }
            onPressed: {
                /* index is not valid yet because the mouse area is not
                   sensitive to hovering (if it were, it would eat hover events
                   for other mouse areas below, which is not desired). */
                var coord = mapToItem(main.contentItem, mouse.x, mouse.y)
                currentIndex = main.indexAt(coord.x, coord.y)
                longPressDelay.start()
            }
            function drop() {
                longPressDelay.stop()
                currentId = ""
                parent.interactive = true
            }
            onReleased: {
                if (currentId != "") {
                    drop()
                } else {
                    /* Forward the click to the launcher item below. */
                    var point = mapToItem(main.contentItem, mouse.x, mouse.y)
                    var item = main.contentItem.childAt(point.x, point.y)
                    /* FIXME: the coordinates of the mouse event forwarded are
                       incorrect. Luckily, it’s acceptable as they are not used in
                       the handler anyway. */
                    if (item && typeof(item.clicked) == "function") item.clicked(mouse)
                }
            }
            onExited: drop()
            onPositionChanged: {
                if (currentId != "" && index != -1 && index != currentIndex) {
                    /* Workaround a bug in QML whereby moving an item down in
                       the list results in its visual representation being
                       shifted too far down by one index
                       (http://bugreports.qt.nokia.com/browse/QTBUG-15841).
                       Since the bug happens only when moving an item *down*,
                       and since moving an item one index down is strictly
                       equivalent to moving the item below one index up, we
                       achieve the same result by tricking the list model into
                       thinking that the mirror operation was performed.
                       Note: this bug will be fixed in Qt 4.7.2, at which point
                       this workaround can go away. */
                    if (index > currentIndex) {
                        items.move(index, currentIndex, 1)
                    } else {
                        /* This should be the only code path here, if it wasn’t
                           for the bug explained and worked around above. */
                        items.move(currentIndex, index, 1)
                    }
                    currentIndex = index
                }
            }
        }
    }

    LauncherList {
        id: shelf
        anchors.bottom: parent.bottom;
        height: tileSize * count + spacing * Math.max(0, count - 1)
        width: parent.width
        /* Ensure the tiles in the shelf are always above those in 'main'. */
        itemZ: 1

        model: ListAggregatorModel {
            id: shelfItems
        }
    }

    LauncherApplicationsList {
        id: applications
    }

    LauncherPlacesList {
        id: places
    }

    LauncherDevicesList {
        id: devices
    }

    WorkspacesList {
        id: workspaces
    }

    Trashes {
        id: trashes
    }

    Component.onCompleted: {
        items.appendModel(applications);
        items.appendModel(workspaces);
        items.appendModel(places);
        items.appendModel(devices);
        shelfItems.appendModel(trashes);
    }

    Connections {
        target: launcherView
        onDesktopFileDropped: applications.insertFavoriteApplication(path)
        onWebpageUrlDropped: applications.insertWebFavorite(url)
    }

    Connections {
        target: launcherControl
        onAddWebFavorite: applications.insertWebFavorite(url)
    }
}
