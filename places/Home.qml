import Qt 4.7

Item {
    Flow {
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.verticalCenter: parent.verticalCenter

        width: 692
        height: 348
        spacing: 60

        HomeButton {
            icon: "image://icons/unity-icon-theme/web"
            label: "Web"
        }
        HomeButton {
            icon: "image://icons/unity-icon-theme/music"
            label: "Music"
        }
        HomeButton {
            icon: "image://icons/unity-icon-theme/photos"
            label: "Photos & Videos"
        }
        HomeButton {
            icon: "image://icons/unity-icon-theme/games"
            label: "Games"
        }
        HomeButton {
            icon: "image://icons/unity-icon-theme/email_and_chat"
            label: "Email & Chat"
        }
        HomeButton {
            icon: "image://icons/unity-icon-theme/work"
            label: "Office"
        }
        HomeButton {
            icon: "image://icons/unity-icon-theme/filesandfolders"
            label: "Files & Folders"
        }
        HomeButton {
            icon: "image://icons/unity-icon-theme/softwarecentre"
            label: "Get New Apps"
        }
    }
}
