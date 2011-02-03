import Qt 4.7
import "utils.js" as Utils
import UnityApplications 1.0
import Unity2d 1.0

/* The main component that manages the windows.
   This only acts as an outer shell, the actual logic is pretty much all in SpreadLayout.qml
   and SpreadItem.qml

   In the rest of the comments there will be some recurring terms that I explain below:
   - screen mode: in this mode each shot is positioned and scaled exactly as the real window.
   - spread mode: in this mode each shot is constrained to a cell in a layout.
   - intro animation: the animation that moves the shots from screen to spread mode
   - outro animation: the animation that moves the shots from spread mode back to screen mode

   The context property called control is the initiator of the entire spread process, and
   is triggered by D-Bus calls on the C++ side.

   The ScreenInfo's property availableGeometry represents the available space on the screen (i.e.
   screen minus launcher, panels, etc.).
*/

GridView {
    id: windows

    signal clicked
    signal windowActivated(variant window)

    MouseArea {
        anchors.fill: parent
        onClicked: windows.clicked()
        /* Eating all mouse events so that they are not passed beneath the workspace */
        hoverEnabled: true
    }

    /* This proxy model takes care of removing all windows that are not on
       the current workspace and that are not pinned to all workspaces. */
    QSortFilterProxyModelQML {
        id: filteredByWorkspace
        model: switcher.allWindows
        dynamicSortFilter: true

        filterRole: WindowInfo.RoleWorkspace
        filterRegExp: RegExp("^%1|-2$".arg(workspace.workspaceNumber))
    }

    /* If there's any application filter set, this proxy model will remove
       all windows that do not belong to it. */
    QSortFilterProxyModelQML {
        id: filteredByApplication
        model: filteredByWorkspace
        dynamicSortFilter: true

        filterRole: WindowInfo.RoleDesktopFile
        filterRegExp: RegExp("%1".arg(switcher.applicationFilter))
    }

    property int columns: Math.ceil(Math.sqrt(count))
    property int rows: Math.ceil(count / columns)

    cellWidth: Math.floor(width / columns)
    cellHeight: height / rows

    model: filteredByApplication

    delegate:
        Item {
            id: cell

            /* Workaround http://bugreports.qt.nokia.com/browse/QTBUG-15642 where onAdd is not called for the first item */
            //GridView.onAdd:
            Component.onCompleted: if (spreadWindow.state == "spread") addAnimation.start()
            GridView.onRemove: if (spreadWindow.state == "spread") removeAnimation.start()

            width: windows.cellWidth
            height: windows.cellHeight

            Keys.onPressed: {
                switch (event.key) {
                    case Qt.Key_Enter:
                    case Qt.Key_Return:
                        windows.windowActivated(spreadWindow)
                        event.accepted = true
                }
            }

            Window {
                id: spreadWindow

                property bool animateFollow: false
                property bool followCell: true

                isSelected: cell.activeFocus

                onEntered: {
                    windows.currentIndex = index
                    cell.forceActiveFocus()
                }

                onClicked: windows.windowActivated(spreadWindow)

                /* Reparenting hack inspired by http://developer.qt.nokia.com/wiki/Drag_and_Drop_within_a_GridView */
                parent: windows

                cellWidth: followCell ? cell.width : cellWidth
                cellHeight: followCell ? cell.height : cellHeight
                z: window.z

                Behavior on x { enabled: spreadWindow.animateFollow; NumberAnimation { duration: Utils.transitionDuration; easing.type: Easing.InOutQuad } }
                Behavior on y { enabled: spreadWindow.animateFollow; NumberAnimation { duration: Utils.transitionDuration; easing.type: Easing.InOutQuad } }
                Behavior on cellWidth { enabled: spreadWindow.animateFollow; NumberAnimation { duration: Utils.transitionDuration; easing.type: Easing.InOutQuad } }
                Behavior on cellHeight { enabled: spreadWindow.animateFollow; NumberAnimation { duration: Utils.transitionDuration; easing.type: Easing.InOutQuad } }

                windowInfo: window
                state: windows.state
                states: [
                    State {
                        name: "screen"
                        PropertyChanges {
                            target: spreadWindow
                            /* Note that we subtract the availableGeometry x and y since window.location is
                            expressed in global screen coordinates. */
                            x: window.position.x - screen.availableGeometry.x
                            y: window.position.y - screen.availableGeometry.y
                            width: window.size.width
                            height: window.size.height
                            animateFollow: false
                        }
                    },
                    State {
                        name: "spread"
                        PropertyChanges {
                            target: spreadWindow
                            /* Center the window in its cell */
                            x: followCell ? (cell.x + (cell.width - spreadWidth) / 2) : x
                            y: followCell ? (cell.y + (cell.height - spreadHeight) / 2) : y
                            width: spreadWidth
                            height: spreadHeight
                            animateFollow: true
                        }
                    }
                ]

                SequentialAnimation {
                    id: addAnimation

                    PropertyAction { target: spreadWindow; property: "animateFollow"; value: false }
                    NumberAnimation { target: spreadWindow; property: "opacity"; from: 0; to: 1.0; duration: Utils.transitionDuration; easing.type: Easing.InOutQuad }
                    PropertyAction { target: spreadWindow; property: "animateFollow"; value: true }
                }
                SequentialAnimation {
                    id: removeAnimation

                    PropertyAction { target: cell; property: "GridView.delayRemove"; value: true }
                    PropertyAction { target: spreadWindow; property: "followCell"; value: false }
                    NumberAnimation { target: spreadWindow; property: "opacity"; to: 0; duration: Utils.transitionDuration; easing.type: Easing.InOutQuad }
                    PropertyAction { target: cell; property: "GridView.delayRemove"; value: false }
                }
                transitions: [
                    Transition {
                        SequentialAnimation {
                            PropertyAction { target: spreadWindow; property: "animating"; value: true }
                            NumberAnimation {
                                properties: "x,y,width,height"
                                duration: Utils.transitionDuration
                                easing.type: Easing.InOutQuad
                            }
                            /* Apply final value to spreadWindow.animateFollow by not specifying a value */
                            PropertyAction { property: "animateFollow" }
                            PropertyAction { target: spreadWindow; property: "animating"; value: false }
                        }
                    }
                ]
            }
    }
}
