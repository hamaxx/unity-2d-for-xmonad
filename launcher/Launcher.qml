import Qt 4.7
import UnityApplications 1.0

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
        anchors {
            top: parent.top; bottom: shelf.top
            topMargin: 5; bottomMargin: 5
        }
        width: parent.width

        autoScrollSize: 18
        autoScrollVelocity: 200

        model: ListAggregatorModel {
            id: items
        }
    }

    LauncherList {
        id: shelf
        anchors {
            bottom: parent.bottom;
            topMargin: 5; bottomMargin: 5
        }
        height: childrenRect.height
        width: parent.width

        autoScrollSize: 0

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
