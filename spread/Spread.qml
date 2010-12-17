import Qt 4.7
import UnityApplications 1.0
import UnityPlaces 1.0

Item {
    width: desktop.width
    height: desktop.height

    GnomeBackground {
        anchors.fill: parent
        overlay_color: "black"
        overlay_alpha: 0
    }

    WindowsList {
        id: wins
        applicationId: control.appId
        onLoaded: grid.state = "spread"
    }

    SpreadGrid {
        id: grid
        items: wins

        onSpreadFinished: {
            control.hide()
            wins.unload()
            control.inProgress = false
        }
    }

    Component.onCompleted: {
        wins.setAppAsPager()
    }

    Connections {
        target: control

        onActivateSpread: {
            control.inProgress = true;
            console.log("Spread activated via control")
            control.show();
            wins.load()
        }

        onCancelSpread: {
            grid.state = ""
        }
    }
}
