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
    width: availableGeometry.width
    height: availableGeometry.height

    GnomeBackground {
        anchors.fill: parent
        overlay_color: "black"
        overlay_alpha: 0
    }

    /* List of windows that will be shown in the spread. */
    WindowsList {
        id: windows
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
        windows: windows

        onTransitionCompleted: {
            if (layout.state == "") {
                spreadView.hide()

                /* If there's any selected window activate it, then clean up the selection */
                if (layout.selectedWindow) {
                    layout.selectedWindow.windowInfo.activate()
                    layout.selectedWindow = null
                }
                windows.unload()
            }
        }
    }

    function cancelSpread() {
        /* When the spread is canceled we do not want to keep focused whatever
           window was focused before the spread started */
        layout.selectedWindow = null;
        layout.state = ""
    }

    Connections {
        target: control

        /* If there's a spread already in progress, cancel it.
           Otherwise start the spread once the windows in the
           model are loaded. */
        onActivateSpread: {
            if (layout.state == "spread") {
                layout.state = ""
            } else {
                /* Please note that the windows list needs to be loaded before the
                   spread view is shown, otherwise windows.lastActiveWindow will not
                   be correct */
                windows.load(applicationId)
                spreadView.show()
                spreadView.forceActivateWindow()
                layout.state = "spread"
            }
        }

        onCancelSpread: cancelSpread()
    }

    Connections {
        target: spreadView
        onOutsideClick: cancelSpread()
    }
}
