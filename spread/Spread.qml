import Qt 4.7
import UnityApplications 1.0
import UnityPlaces 1.0

/* The main component that manages the spread.
   This only acts as an outer shell, the actual logic is pretty much all in SpreadGrid.qml
   and SpreadItem.qml

   In the rest of the comments there will be some recurring terms that I explain below:
   - shot: a screenshot of the window, minus the decorations. it also refers to the rectangle
           with an icon inside that takes its place when the screenshot can't be grabbed.
   - screen mode: in this mode each shot is positioned and scaled exactly as the real window.
   - grid mode: in this mode each shot is constrained to a cell in a grid.
   - intro animation: the animation that moves the shots from screen to grid mode
   - outro animation: the animation that moves the shots from grid mode back to screen mode

   The context property called control is the initiator of the entire spread process, and
   is triggered by a DBus call on the c++ side. It has a "inProgress" property that is
   true from the start of the intro animation to the end of the outro animation.

   The context property called availableGeometry represents the available space on the screen (i.e.
   screen minus launcher, panels, etc)
*/

Item {
    width: availableGeometry.width
    height: availableGeometry.height

    // Just displays the current gnome background below the spread
    GnomeBackground {
        anchors.fill: parent
        overlay_color: "black"
        overlay_alpha: 0
    }

    /* This is the main model, that contains a list of the windows
       that will partecipate in the expose and their properties.
       After it finishes loading it fires the loaded() signal, which
       triggers the intro animation (which is tied to the grid's state) */
    WindowsList {
        id: windows
        applicationId: control.appId
        onLoaded: grid.state = "spread"
    }

    /* This is our main view.
       It's essentially just a container where we do our own positioning of
       the children.
       It had two states: the default one (named "") where it positions the
       items according to screen mode. And the other named "spread" where
       the items are positioned according to grid mode. */
    SpreadGrid {
        id: grid
        windows: windows

        // This is triggered after an outro animation is fully complete
        onSpreadFinished: {
            control.hide()  // Hides the entire window
            windows.unload()   // Avoid wasting memory by keeping old shots
            control.inProgress = false
        }
    }

    Connections {
        target: control

        // This is fired as a result of a method call from DBus
        onActivateSpread: {
            control.inProgress = true;
            control.show(); // Shows the entire window
            windows.load()     // Start taking shots of the windows
        }

        // Same as above, but if any spread is in progress it will start
        // playing the outro animation no matter what the current state is
        onCancelSpread: grid.state = ""
    }

    Component.onCompleted: {
        // See places/UnityPlaces/WindowsList.cpp for
        // an explanation of what this is and why it shouldn't be here.
        windows.setAppAsPager()
    }
}
