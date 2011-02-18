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

        paddingTop: 5
        paddingBottom: 5
        autoScrollSize: tileSize / 2
        autoScrollVelocity: 200

        model: ListAggregatorModel {
            id: items
        }
    }

    LauncherList {
        id: shelf
        anchors.bottom: parent.bottom;
        height: tileSize * count + spacing * Math.max(0, count - 1)
        width: parent.width

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
