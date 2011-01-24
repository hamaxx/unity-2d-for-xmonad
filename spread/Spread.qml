import Qt 4.7
import UnityApplications 1.0
import UnityPlaces 1.0

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

   The context property called availableGeometry represents the available space on the screen (i.e.
   screen minus launcher, panels, etc.).
*/

Item {
    id: spread
    width: availableGeometry.width
    height: availableGeometry.height

    property string application

    property alias state: layout.state
    property alias selectedXid: layout.selectedXid

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

    QSortFilterProxyModelQML {
        id: filteredByApplication
        model: switcher.allWindows
        dynamicSortFilter: true

        filterRole: WindowInfo.RoleDesktopFile
        filterRegExp: RegExp("%1".arg(application))
    }

    QSortFilterProxyModelQML {
        id: filteredByWorkspace
        model: filteredByApplication
        dynamicSortFilter: true

        filterRole: WindowInfo.RoleWorkspace
        filterRegExp: RegExp("^%1|-2$".arg(workspace.workspaceNumber))
    }

    /* This is our main view.
       It's essentially just a container where we do our own positioning of
       the windows.
       It has two states: the default one (named "") where it positions the
       items according to screen mode. And the other named "spread" where
       the items are positioned according to spread mode. */
    SpreadLayout {
        id: layout

        anchors.fill: parent
        windows: filteredByWorkspace

        onWindowActivated: {
            layout.raiseSelectedWindow()
            spread.windowActivated()
        }
    }

    function windowForXid(xid)
    {
        var children = layout.children
        for (var i = 0; i < children.length; i++) {
            var child = children[i]
            if (child.windowInfo && child.windowInfo.contentXid == xid) {
                return child;
            }
        }
        return null;
    }
}
