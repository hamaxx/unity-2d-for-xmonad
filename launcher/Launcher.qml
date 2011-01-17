import Qt 4.7
import UnityApplications 1.0

Item {
    width: 58
    height: 1024

    Image {
        id: background

        anchors.fill: parent
        fillMode: Image.TileVertically
        source: "/usr/share/unity/themes/launcher_background_middle.png"
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

            width: 58; height: 54
            icon: "image://icons/"+item.icon
            running: item.running
            active: item.active
            urgent: item.urgent
            launching: item.launching

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
                item.menu.show(width-4, y+height/2-list.contentY+launcherView.y)
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
            onEntered: if (!list.moving) showMenu()
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
            }

            function setIconGeometry() {
                if (running) {
                    item.setIconGeometry(x + launcherView.x, y + launcherView.y, width, height)
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
                onWindowAdded: item.setIconGeometry(x + launcherView.x, y + launcherView.y, width, height, xid)
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

    Component.onCompleted: {
        items.appendModel(applications);
        items.appendModel(places);
        items.appendModel(devices);
        items.appendModel(trashes);
    }

    Connections {
        target: launcherView
        onDesktopFileDropped: applications.insertFavoriteApplication(path)
    }

    Connections {
        target: launcherControl
        onAddWebFavorite: applications.insertWebFavorite(url)
    }
}
