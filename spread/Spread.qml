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
        onLoaded: {
            grid.items = wins
            grid.state = "spread"
        }
    }

    SpreadGrid {
        id: grid

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

    MouseArea {
        width: 100
        height: 100
        anchors.top: parent.top
        anchors.left: parent.left
        onDoubleClicked: Qt.quit()
    }
}
