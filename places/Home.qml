import Qt 4.7
import UnityApplications 1.0 /* Necessary for the ImageProvider serving image://icons/theme_name/icon_name */
import gconf 1.0

Page {
    Flow {
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.verticalCenter: parent.verticalCenter

        width: 692
        height: 348
        spacing: 60

        HomeButton {
            icon: "image://icons/unity-icon-theme/web"
            label: qsTr("Web")

            GConfItem {
                id: desktop_file_path
                key: "/desktop/gnome/applications/browser/exec"
            }

            onClicked: {
                dashView.active = false
                console.log("FIXME: should launch", desktop_file_path.value)
                Qt.openUrlExternally("http://")
            }
        }

        HomeButton {
            icon: "image://icons/unity-icon-theme/music"
            label: qsTr("Music")
            onClicked: activatePlace(applications_place, 4)
        }

        HomeButton {
            icon: "image://icons/unity-icon-theme/photos"
            label: qsTr("Photos & Videos")
            onClicked: activatePlace(applications_place, 4)
        }

        HomeButton {
            icon: "image://icons/unity-icon-theme/games"
            label: qsTr("Games")
            onClicked: activatePlace(applications_place, 2)
        }

        HomeButton {
            icon: "image://icons/unity-icon-theme/email_and_chat"
            label: qsTr("Email & Chat")
            onClicked: activatePlace(applications_place, 3)
        }

        HomeButton {
            icon: "image://icons/unity-icon-theme/work"
            label: qsTr("Office")
            onClicked: activatePlace(applications_place, 5)
        }

        HomeButton {
            icon: "image://icons/unity-icon-theme/filesandfolders"
            label: qsTr("Files & Folders")
            onClicked: activatePlace(files_place, 0)
        }

        HomeButton {
            icon: "image://icons/unity-icon-theme/softwarecentre"
            label: qsTr("Get New Apps")

            QLauncherApplication {
                id: software_center
                desktop_file: "/usr/share/applications/ubuntu-software-center.desktop"
            }

            onClicked: {
                dashView.active = false
                software_center.launch()
            }
        }
    }
}
