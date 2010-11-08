import Qt 4.7

/* Item displaying a launcher item.

   It contains:
    - a generic bordered background image
    - an icon representing the item
    - a text describing the item

   When an application is launched, the border changes appearance.
   It supports mouse hover by changing the appearance of the background image.

   The 'icon' property holds the source of the image to load as an icon.
   The 'label' property holds the text to display.
   The 'running' property is a boolean indicating whether or not the
   application is launched.

   The 'clicked' signal is emitted upon clicking on the item.
*/
Item {
    property alias icon: icon.source
    property alias label: label.text
    property bool running: false
    property bool active: false
    property bool urgent: false
    property bool launching: false

    signal clicked(variant mouse)
    signal entered
    signal exited

    MouseArea {
        id: mouse

        acceptedButtons: Qt.LeftButton | Qt.RightButton
        hoverEnabled: true
        anchors.fill: parent
        onClicked: parent.clicked(mouse)
        onEntered: parent.entered()
        onExited: parent.exited()
    }

    Keys.onPressed: {
        if (event.key == Qt.Key_Return) {
            clicked()
            event.accepted = true;
        }
    }

    Image {
        id: shadow

        source: "artwork/shadow.png"
        asynchronous: true
    }

    Image {
        id: glow

        anchors.horizontalCenter: parent.horizontalCenter
        anchors.verticalCenter: parent.verticalCenter

        source: "artwork/glow.png"
        asynchronous: true
        opacity: 0.0

        SequentialAnimation on opacity {
            loops: Animation.Infinite
            alwaysRunToEnd: true
            running: launching
            NumberAnimation { to: 1.0; duration: 1000; easing.type: Easing.InOutQuad }
            NumberAnimation { to: 0.0; duration: 1000; easing.type: Easing.InOutQuad }
        }
    }

    Item {
        id: container

        width: 50
        height: 50
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.verticalCenter: parent.verticalCenter

        Rectangle {
            id: background

            opacity: mouse.containsMouse ? 1.0 : 0.9
            anchors.fill: parent
            anchors.margins: 1
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.verticalCenter: parent.verticalCenter
            smooth: true
            color: if(icon.source != "")
                       return launcherView.iconAverageColor(icon.source,
                                                            Qt.size(icon.width, icon.height))
            radius: 5
        }

        Image {
            id: icon

            width: 30
            height: 30
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.verticalCenter: parent.verticalCenter
            fillMode: Image.PreserveAspectFit
            sourceSize.width: width
            sourceSize.height: height
            smooth: true

            asynchronous: true
            opacity: status == Image.Ready ? 1 : 0
            Behavior on opacity {NumberAnimation {duration: 200; easing.type: Easing.InOutQuad}}
        }

        Image {
            id: foreground

            anchors.fill: parent
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.verticalCenter: parent.verticalCenter
            fillMode: Image.PreserveAspectFit
            sourceSize.width: width
            sourceSize.height: height
            smooth: true

            source: "/usr/share/unity/themes/prism_icon_foreground.png"

            asynchronous: true
            opacity: status == Image.Ready ? 1 : 0
            Behavior on opacity {NumberAnimation {duration: 200; easing.type: Easing.InOutQuad}}
        }

        SequentialAnimation {
            id: nudging
            running: urgent
            SequentialAnimation {
                loops: 30
                NumberAnimation { target: container; property: "rotation"; to: 15; duration: 150 }
                NumberAnimation { target: container; property: "rotation"; to: -15; duration: 150 }
            }
            NumberAnimation { target: container; property: "rotation"; to: 0; duration: 75 }
        }

        NumberAnimation {
            id: end_nudging
            running: !urgent
            target: container
            property: "rotation"
            to: 0
            duration: 75
        }

    }

    Image {
        id: running_arrow

        z: -1
        width: sourceSize.width
        height: sourceSize.height
        anchors.rightMargin: -2
        anchors.right: container.left
        anchors.verticalCenter: container.verticalCenter
        opacity: running ? 1.0 : 0.0
        source: urgent ? "/usr/share/unity/themes/application-running-notify.png" : "/usr/share/unity/themes/application-running.png"

        Behavior on opacity {NumberAnimation {duration: 200; easing.type: Easing.InOutQuad}}
    }

    Image {
        id: active_arrow

        z: -1
        width: sourceSize.width
        height: sourceSize.height
        anchors.leftMargin: -2
        anchors.left: container.right
        anchors.verticalCenter: container.verticalCenter
        opacity: active ? 1.0 : 0.0
        source: "/usr/share/unity/themes/application-selected.png"

        Behavior on opacity {NumberAnimation {duration: 200; easing.type: Easing.InOutQuad}}
    }

    Text {
        id: label

        font.pointSize: 10
        wrapMode: Text.WordWrap
        horizontalAlignment: Text.AlignHCenter
        anchors.top: parent.bottom
        anchors.topMargin: 7
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        font.underline: parent.focus
    }
}
