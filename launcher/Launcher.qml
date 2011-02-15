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

    AutoScrollingListView {
        id: list

        property int spacingSize: 5
        /* This blank header is actually a replacement for anchors.topMargin.
           We are doing it this way because the margins are outside of the item,
           and thus would not be covered by the top MouseArea inside of the
           ScrollingListView. The header is inside instead, so it is covered. */
        header: Item { height: list.spacingSize }
        spacing: spacingSize

        anchors.fill: parent
        focus: true
        property int itemHeight: 54
        autoScrollSize: itemHeight / 2
        autoScrollVelocity: 200

        /* Keep a reference to the currently visible contextual menu */
        property variant visibleMenu

        model: ListAggregatorModel {
            id: items
        }

        delegate: LauncherItem {
            id: launcherItem

            width: launcher.width
            height: tileSize

            icon: "image://icons/" + item.icon
            running: item.running
            active: item.active
            urgent: item.urgent
            launching: item.launching
            pips: Math.min(item.windowCount, 3)
            tileSize: list.itemHeight

            /* Best way I could find to check if the item is an application or the
               workspaces switcher. There may be something cleaner and better. */
            backgroundFromIcon: item.toString().indexOf("LauncherApplication") == 0 ||
                                item.toString().indexOf("Workspaces") == 0

            Binding { target: item.menu; property: "title"; value: item.name }

            function showMenu() {
                /* Prevent the simultaneous display of multiple menus */
                if (list.visibleMenu != item.menu && list.visibleMenu != undefined) {
                    list.visibleMenu.hide()
                }
                list.visibleMenu = item.menu
                // The extra 4 pixels are needed to center exactly with the arrow
                // that indicated the active tile.
                item.menu.show(width,
                               y + height / 2 - list.contentY + panel.y - list.spacingSize + 4)
            }

            onClicked: {
                if (mouse.button == Qt.LeftButton) {
                    item.menu.hide()
                    item.activate()
                }
                else if (mouse.button == Qt.RightButton) {
                    item.menu.folded = false
                    showMenu()
                }
            }

            /* Display the tooltip when hovering the item only when the list
               is not moving */
            onEntered: if (!list.moving && !list.autoScrolling) showMenu()
            onExited: {
                /* When unfolded, leave enough time for the user to reach the
                   menu. Necessary because there is some void between the item
                   and the menu. Also it fixes the case when the user
                   overshoots. */
                if (!item.menu.folded)
                    item.menu.hideWithDelay(400)
                else
                    item.menu.hide()
            }

            Connections {
                target: list
                onMovementStarted: item.menu.hide()
                onAutoScrollingChanged: if (list.autoScrolling) item.menu.hide()
            }

            function setIconGeometry() {
                if (running) {
                    item.setIconGeometry(x + panel.x, y + panel.y, width, height)
                }
            }

            ListView.onAdd: SequentialAnimation {
                PropertyAction { target: launcherItem; property: "scale"; value: 0 }
                NumberAnimation { target: launcherItem; property: "height";
                                  from: 0; to: launcherItem.tileSize; duration: 250; easing.type: Easing.InOutQuad }
                NumberAnimation { target: launcherItem; property: "scale"; to: 1; duration: 250; easing.type: Easing.InOutQuad }
            }

            ListView.onRemove: SequentialAnimation {
                PropertyAction { target: launcherItem; property: "ListView.delayRemove"; value: true }
                NumberAnimation { target: launcherItem; property: "scale"; to: 0; duration: 250; easing.type: Easing.InOutQuad }
                NumberAnimation { target: launcherItem; property: "height"; to: 0; duration: 250; easing.type: Easing.InOutQuad }
                PropertyAction { target: launcherItem; property: "ListView.delayRemove"; value: false }
            }

            onRunningChanged: setIconGeometry()
            /* Note: this doesn’t work as expected for the first favorite
               application in the list if it is already running when the
               launcher is started, because its y property doesn’t change.
               This isn’t too bad though, as the launcher is supposed to be
               started before any other regular application. */
            onYChanged: setIconGeometry()

            Connections {
                target: item
                onWindowAdded: item.setIconGeometry(x + panel.x, y + panel.y, width, height, xid)
                /* Not all items are applications. */
                ignoreUnknownSignals: true
            }
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

    Trashes {
        id: trashes
    }

    WorkspacesList {
        id: workspaces
    }

    Component.onCompleted: {
        items.appendModel(applications);
        items.appendModel(workspaces);
        items.appendModel(places);
        items.appendModel(devices);
        items.appendModel(trashes);
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
