import Qt 4.7
import UnityApplications 1.0
import Unity2d 1.0 /* required for drag’n’drop handling */

DropItem {
    id: launcher

    Image {
        id: background

        anchors.fill: parent
        fillMode: Image.TileVertically
        source: "artwork/background.png"
    }

    onDragEnter: launcherView.onDragEnter(event)
    onDrop: launcherView.onDrop(event)

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
        reorderable: true

        model: ListAggregatorModel {
            id: items
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
