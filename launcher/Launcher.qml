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

    ListView {
        id: list

        /* Keep a reference to the currently visible contextual menu */
        property variant visibleMenu

        anchors.fill: parent
        focus: true

        model: ListAggregatorModel {
            id: items
        }

        delegate: LauncherItem {
            id: wrapper

            width: launcher.width
            height: 54 + 5

            icon: "image://icons/"+item.icon
            running: item.running
            active: item.active
            urgent: item.urgent
            launching: item.launching

            anchors.horizontalCenter: parent.horizontalCenter

            Binding { target: item.menu; property: "title"; value: item.name }

            function showMenu() {
                /* Prevent the simultaneous display of multiple menus */
                if (list.visibleMenu != item.menu && list.visibleMenu != undefined) {
                    list.visibleMenu.hide()
                }
                list.visibleMenu = item.menu

                /* The menu needs to never overlap with the MouseArea of
                   item otherwise flickering happens when the mouse is on
                   an overlapping pixel (hence the -4). */
                item.menu.show(width-4, y+height/2-list.contentY+panel.y)
            }

            Connections {
                target: list
                onMovementStarted: item.menu.hide()
            }

            function setIconGeometry() {
                if (running) {
                    item.setIconGeometry(x + panel.x, y + panel.y, width, height)
                }
            }

            ListView.onAdd: SequentialAnimation {
                PropertyAction { target: wrapper; property: "scale"; value: 0 }
                NumberAnimation { target: wrapper; property: "height"; from: 0; to: 54; duration: 250; easing.type: Easing.InOutQuad }
                NumberAnimation { target: wrapper; property: "scale"; to: 1; duration: 250; easing.type: Easing.InOutQuad }
            }

            ListView.onRemove: SequentialAnimation {
                PropertyAction { target: wrapper; property: "ListView.delayRemove"; value: true }
                NumberAnimation { target: wrapper; property: "scale"; to: 0; duration: 250; easing.type: Easing.InOutQuad }
                NumberAnimation { target: wrapper; property: "height"; to: 0; duration: 250; easing.type: Easing.InOutQuad }
                PropertyAction { target: wrapper; property: "ListView.delayRemove"; value: false }
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
    }
}
