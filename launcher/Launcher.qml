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

        // FIXME: flicking the list fast exhibits unpleasant visual artifacts:
        // the y coordinate of the looseItems is correct, however they are not
        // re-drawn at the correct position until the mouse cursor is moved
        // further. This may be a bug in QML.
        MouseArea {
            /* Handle drag’n’drop to re-order applications. */
            id: dnd
            anchors.fill: parent

            /* list index of the tile being dragged */
            property int draggedTileIndex
            /* id (desktop file path) of the tile being dragged */
            property string draggedTileId: ""
            /* absolute mouse coordinates in the list */
            property variant listCoordinates: mapToItem(main.contentItem, mouseX, mouseY)
            /* list index of the tile underneath the cursor */
            property int tileAtCursorIndex: main.indexAt(listCoordinates.x, listCoordinates.y)

            Timer {
                id: longPressDelay
                /* The standard threshold for long presses is hard-coded to 800ms
                   (http://doc.qt.nokia.com/qml-mousearea.html#onPressAndHold-signal).
                   This value is too high for our use case. */
                interval: 500 /* in milliseconds */
                onTriggered: {
                    if (main.moving) return
                    dnd.parent.interactive = false
                    var id = items.get(dnd.draggedTileIndex).desktop_file
                    if (id != undefined) dnd.draggedTileId = id
                }
            }
            onPressed: {
                /* tileAtCursorIndex is not valid yet because the mouse area is
                   not sensitive to hovering (if it were, it would eat hover
                   events for other mouse areas below, which is not desired). */
                var coord = mapToItem(main.contentItem, mouse.x, mouse.y)
                draggedTileIndex = main.indexAt(coord.x, coord.y)
                longPressDelay.start()
            }
            function drop() {
                longPressDelay.stop()
                draggedTileId = ""
                parent.interactive = true
            }
            onReleased: {
                if (draggedTileId != "") {
                    drop()
                } else if (draggedTileIndex == tileAtCursorIndex) {
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
                if (draggedTileId != "" && tileAtCursorIndex != -1 && tileAtCursorIndex != draggedTileIndex) {
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
                    if (tileAtCursorIndex > draggedTileIndex) {
                        items.move(tileAtCursorIndex, draggedTileIndex, 1)
                    } else {
                        /* This should be the only code path here, if it wasn’t
                           for the bug explained and worked around above. */
                        items.move(draggedTileIndex, tileAtCursorIndex, 1)
                    }
                    draggedTileIndex = tileAtCursorIndex
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
