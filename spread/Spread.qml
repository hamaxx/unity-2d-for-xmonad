import Qt 4.7
import UnityApplications 1.0
import UnityPlaces 1.0
import "utils.js" as Utils

/* The main component that manages the spread.
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

Item {
    id: spread
    width: screen.availableGeometry.width
    height: screen.availableGeometry.height

    property string application

//    property alias selectedXid: layout.selectedXid

    signal windowActivated

    GnomeBackground {
        anchors.fill: parent
        overlay_color: "black"
        overlay_alpha: 0

        MouseArea {
            anchors.fill: parent
            onClicked: {
                spread.selectedXid = 0
                spread.windowActivated()
            }
        }
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

    GridView {
        id: grid
        anchors.fill: parent

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
                Component.onCompleted: if (spreadWindow.state != "screen") addAnimation.start()
                GridView.onRemove: removeAnimation.start()

                width: grid.cellWidth
                height: grid.cellHeight

                SpreadWindow {
                    id: spreadWindow

                    property bool animateFollow
                    property bool followCell: true

                    /* Reparenting hack inspired by http://developer.qt.nokia.com/wiki/Drag_and_Drop_within_a_GridView */
                    parent: grid

                    cellWidth: followCell ? cell.width : cellWidth
                    cellHeight: followCell ? cell.height : cellHeight
                    z: window.z

                    Behavior on x { enabled: spreadWindow.animateFollow; NumberAnimation { duration: Utils.transitionDuration; easing.type: Easing.InOutQuad } }
                    Behavior on y { enabled: spreadWindow.animateFollow; NumberAnimation { duration: Utils.transitionDuration; easing.type: Easing.InOutQuad } }
                    Behavior on cellWidth { enabled: spreadWindow.animateFollow; NumberAnimation { duration: Utils.transitionDuration; easing.type: Easing.InOutQuad } }
                    Behavior on cellHeight { enabled: spreadWindow.animateFollow; NumberAnimation { duration: Utils.transitionDuration; easing.type: Easing.InOutQuad } }

                    windowInfo: window
                    state: spread.state
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
                                x: followCell ? cell.x : x
                                y: followCell ? cell.y : y
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
                            from: "screen"
                            to: "spread"
                            SequentialAnimation {
                                NumberAnimation {
                                    properties: "x,y,width,height"
                                    duration: Utils.transitionDuration
                                    easing.type: Easing.InOutQuad
                                }
                                /* Apply final value to spreadWindow.animateFollow by not specifying a value */
                                PropertyAction { property: "animateFollow" }
                            }
                        }
                    ]
                }
        }
    }

    function windowForXid(xid)
    {
        var children = grid.children
        for (var i = 0; i < children.length; i++) {
            var child = children[i]
            if (child.windowInfo && child.windowInfo.contentXid == xid) {
                return child;
            }
        }
        return null;
    }
}
