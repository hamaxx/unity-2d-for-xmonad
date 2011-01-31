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

//    property alias state: layout.state
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

        /* If gaps is > 0, then there are missing cells in the last row.
           In that case, extraSpace is the amount of extra space that is
           available in the last row due to the fact they are missing */
        property int gaps: (columns * rows) - count
        property real extraSpace: (gaps * cellWidth) / (columns - gaps)

        model: filteredByApplication
        delegate: Rectangle {
            id: cell
            color: "transparent"
            border.width: 1

            property int column: index % grid.columns
            property int row: Math.floor(index / grid.columns)

            /* If expand is true, this cell is one of those in the last row that
               need to share the exta space left by the missing cells */
            property bool expand: grid.gaps > 0 && row == (grid.rows - 1)
            property int expandShift: (expand) ? (grid.extraSpace * column)  : 0

            width: grid.cellWidth + ((expand) ? grid.extraSpace : 0)
            height: grid.cellHeight

            transform: Translate { x: expandShift }

            /* When a cell is added or removed, trigger the corresponding window
               animations so that the new window fades in or out smoothly in place. */
            GridView.onAdd: { console.log("ADDED " + index); addAnimation.start() }
            GridView.onRemove: { console.log("REMOVED"); removeAnimation.start() }

            /* When a delegate is about to be removed from a GridView the GridView.onRemove
               signal will be fired, and it's setup to run this animation.
               To make sure that the delegate is not destroyed until the animation is complete
               GridView makes available the property GridView.delayRemove that prevents the
               grid from actually destroying the delegate while it's set to true, and then
               destroys it when set to false. */
            SequentialAnimation {
                id: removeAnimation
                PropertyAction { target: cell; property: "GridView.delayRemove"; value: true }
                ScriptAction { script: cell.enableBehaviors = false; }
                NumberAnimation {
                    target: cellFollower
                    properties: "opacity,scale"
                    to: 0.0;
                    duration: Utils.currentTransitionDuration
                    easing.type: Easing.InOutSine
                }
                PropertyAction { target: cell; property: "GridView.delayRemove"; value: false }
            }

            /* It is very important that we enable the behaviors at the end of the enterAnimation
               so that items will be able to slide around in the grid when other items before them
               are added or removed. */
            SequentialAnimation {
                id: addAnimation
                PropertyAction { target: cellFollower; properties: "opacity, scale"; value: 0.0 }
                NumberAnimation {
                    target: cellFollower
                    properties: "opacity, scale"
                    to: 1.0
                    duration: Utils.currentTransitionDuration
                    easing.type: Easing.InOutSine
                }
                ScriptAction { script: cell.enableBehaviors = true; }
            }

            /* This property should be directly on the cellFollower, but for some reason
               the onAdd transition can't see it (probably because it is already reparented
               by the time the transitions are run, and thus out of the component scope)
               So we need to bind it via a property on the cell */
            property variant enableBehaviors: false

            Item {
                id: cellFollower
                parent: holder

                /* If we expanded this cell to use part of the empty space in the
                   last row, and it's not the first one, we need to shift it left by
                   the amount of extra space consumed by the previous expanded rows. */
                x: cell.x + cell.expandShift
                y: cell.y
                width: cell.width
                height: cell.height

                property alias windowInfo: spreadWindow.windowInfo

                Behavior on x {
                    enabled: cell.enableBehaviors
                    NumberAnimation { easing.type: Easing.InOutSine; duration: Utils.currentTransitionDuration }
                }
                Behavior on y {
                    enabled: cell.enableBehaviors
                    NumberAnimation { easing.type: Easing.InOutSine; duration: Utils.currentTransitionDuration }
                }
                Behavior on width {
                    enabled: cell.enableBehaviors
                    NumberAnimation { easing.type: Easing.InOutSine; duration: Utils.currentTransitionDuration }
                }
                Behavior on height {
                    enabled: cell.enableBehaviors
                    NumberAnimation { easing.type: Easing.InOutSine; duration: Utils.currentTransitionDuration }
                }

                SpreadWindow {
                    id: spreadWindow

                    anchors.centerIn: parent
                    cellWidth: cell.width
                    cellHeight: cell.height

                    state: spread.state

                    windowInfo: window

                    //        onWindowActivated: {
                    //            layout.raiseSelectedWindow()
                    //            spread.windowActivated()
                    //        }
                }
            }
        }
    }

    Item {
        id: holder
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
