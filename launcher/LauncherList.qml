import Qt 4.7
import UnityApplications 1.0
import Unity2d 1.0 /* required for drag’n’drop handling */

AutoScrollingListView {
    id: list

    /* The spacing is explicitly set to 0 and compensated for
       by adding some padding to the items because of
       http://bugreports.qt.nokia.com/browse/QTBUG-17622. */
    spacing: 0
    property int itemPadding: 5

    property int tileSize: 54

    /* Keep a reference to the currently visible contextual menu */
    property variant visibleMenu

    /* A hint for items to determine the value of their 'z' property */
    property real itemZ: 0

    /* Can we reorder the items in this list by means of drag and drop ? */
    property alias reorderable: reorder.enabled

    ListViewDragAndDrop {
        id: reorder
        list: list
        enabled: false
    }

    /* FIXME: We need this only to workaround a problem in QT's MouseArea
       event handling. See AutoScrollingListView for details. */
    dragAndDrop: (reorder.enabled) ? reorder : null

    delegate: LauncherItem {
        id: launcherItem

        width: list.width
        tileSize: list.tileSize
        padding: list.itemPadding

        desktopFile: item.desktop_file ? item.desktop_file : ""
        icon: "image://icons/" + item.icon
        running: item.running
        active: item.active
        urgent: item.urgent
        launching: item.launching
        pips: Math.min(item.windowCount, 3)

        property bool noOverlays: item.counter == undefined
        counter: (noOverlays) ? 0 : item.counter
        counterVisible: (noOverlays) ? false : item.counterVisible
        progress: (noOverlays) ? 0.0 : item.progress
        progressBarVisible: (noOverlays) ? false : item.progressBarVisible
        emblem: (noOverlays && item.emblem) ? "image://icons/" + item.emblem : ""
        emblemVisible: (noOverlays) ? false : item.emblemVisible

        shortcutVisible: item.toString().indexOf("LauncherApplication") == 0 &&
                         index <= 9 && launcherView.superKeyPressed
        shortcutText: index + 1

        isBeingDragged: (reorder.draggedTileId != "") && (reorder.draggedTileId == desktopFile)
        dragPosition: reorder.listCoordinates.y - list.contentY

        /* Best way I could find to check if the item is an application or the
           workspaces switcher. There may be something cleaner and better. */
        backgroundFromIcon: item.toString().indexOf("LauncherApplication") == 0 ||
                            item.toString().indexOf("Workspaces") == 0

        Binding { target: item.menu; property: "title"; value: item.name }

        /* Drag’n’drop handling */
        onDragEnter: item.onDragEnter(event)
        onDrop: item.onDrop(event)

        function showMenu() {
            /* Prevent the simultaneous display of multiple menus */
            if (list.visibleMenu != item.menu && list.visibleMenu != undefined) {
                list.visibleMenu.hide()
            }
            list.visibleMenu = item.menu
            // FIXME: The extra 2 pixels are needed to center the menu arrow with
            // the center of the tile.
            item.menu.show(width, panel.y + list.y - list.contentY +
                                  y + height - tileSize / 2 - 2)

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

        Connections {
            target: reorder
            /* Hide the tooltip/menu when dragging an application. */
            onDraggedTileIdChanged: if (reorder.draggedTileId != "") item.menu.hide()
        }

        function setIconGeometry() {
            if (running) {
                item.setIconGeometry(x + panel.x, y + panel.y, width, height)
            }
        }

        ListView.onAdd: SequentialAnimation {
            PropertyAction { target: launcherItem; property: "scale"; value: 0 }
            NumberAnimation { target: launcherItem; property: "height";
                              from: 0; duration: 250; easing.type: Easing.InOutQuad }
            NumberAnimation { target: launcherItem; property: "scale"; to: 1; duration: 250; easing.type: Easing.InOutQuad }
        }

        ListView.onRemove: SequentialAnimation {
            /* Disable all mouse interactions on the delegate being removed. This prevents a bug where QT itself
               crashes when trying to access some properties of the model item being removed while the item is being
               "kept alive" by ListView.delayRemove.
               In our case the property causing the bug is 'menu', but there may be others, so it's safer to just disable
               all mouse interactions. These interactions should not happen anyway while the tile is animating away
               since there's nothing that the user can do with it anyway: it's for all intents and purposes already gone.
               See: https://bugs.launchpad.net/unity-2d/+bug/719507 */
            PropertyAction { target: launcherItem; property: "interactive"; value: false }

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

        Connections {
            target: launcherView
            onKeyboardShortcutPressed: {
                /* Only applications can be launched by keyboard shortcuts */
                if (item.toString().indexOf("LauncherApplication") == 0 && index == itemIndex) {
                    item.menu.hide()
                    item.activate()
                }
            }
        }
    }
}
