import Qt 4.7

/* Item displaying an application.

   It contains:
    - a generic bordered background image
    - an icon representing the application
    - a text describing the application

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

    signal clicked

    MouseArea {
        hoverEnabled: true
        anchors.fill: parent
        onClicked: parent.clicked()
//        onEntered: background.opacity = 1.0
//        onExited: background.opacity = 0.7
    }

    Keys.onPressed: {
        if (event.key == Qt.Key_Return) {
            clicked()
            event.accepted = true;
        }
    }

    //    Behavior on opacity {NumberAnimation {duration: 200; easing.type: Easing.InOutQuad}}

    Image {
        id: icon

        width: 48
        height: 48
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
        id: running_arrow

        width: sourceSize.width
        height: sourceSize.height
        anchors.right: icon.left
        anchors.verticalCenter: icon.verticalCenter
        opacity: running ? 1.0 : 0.0
        source: "/usr/share/unity/themes/application-running.png"

        Behavior on opacity {NumberAnimation {duration: 200; easing.type: Easing.InOutQuad}}
    }

    Image {
        id: active_arrow

        width: sourceSize.width
        height: sourceSize.height
        anchors.left: icon.right
        anchors.verticalCenter: icon.verticalCenter
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
