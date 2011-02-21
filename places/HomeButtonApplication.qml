import Qt 4.7
/* Necessary for:
   - ImageProvider serving image://icons/theme_name/icon_name
   - LauncherApplication
*/
import UnityApplications 1.0

HomeButton {
    property alias desktopFile: application.desktop_file

    LauncherApplication {
        id: application
    }

    onClicked: {
        dashView.active = false
        application.activate()
    }

    icon: "image://icons/" + application.icon
}
