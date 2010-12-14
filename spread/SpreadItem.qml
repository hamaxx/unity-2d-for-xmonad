import Qt 4.7

Item {
    id: item
    property variant win

    property real darkness: 0.0

    property int column
    property int row

    property int columnsInRow: (row == parent.rows - 1 && parent.lastRowColumns != 0) ?
                                   parent.lastRowColumns : parent.columns
    property real columnWidth: (parent.width - parent.anchors.margins) / columnsInRow

    property bool needsActivation: false

    x: (win.location.x * parent.ratio) - desktop.x
    y: (win.location.y * parent.ratio) - desktop.y
    width: win.size.width * parent.ratio
    height: win.size.height * parent.ratio
    z: win.z

    Item {
        id: box
        anchors.fill: parent

        property real widthScale: width / item.win.size.width
        property real heightScale: height / item.win.size.height
        property real scaledWinWidth: ((widthScale <= heightScale) ? parent.width - anchors.margins * 2: heightScale * item.win.size.width)
        property real scaledWinHeight: ((widthScale <= heightScale) ? widthScale * item.win.size.height : parent.height - anchors.margins * 2)

        MouseArea {
            id: itemArea
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.verticalCenter: parent.verticalCenter

            width: parent.scaledWinWidth
            height: parent.scaledWinHeight

            hoverEnabled: true
            onEntered: if (item.state == "spread") item.darkness = 0.0
            onExited: if (item.state == "spread") item.darkness = 1.0
        }

        Image {
            id: shot
            source: "image://window/" + item.win.xid
            anchors.fill: parent
            fillMode: Image.PreserveAspectFit
            smooth: true

            opacity: (status == Image.Error) ? 0.0 : 1.0
        }

        Rectangle {
            id: window_box
            opacity: (shot.status == Image.Error) ? 1.0 : 0.0

            width: parent.scaledWinWidth
            height: parent.scaledWinHeight

            anchors.horizontalCenter: parent.horizontalCenter
            anchors.verticalCenter: parent.verticalCenter

            border.width: 1
            border.color: "black"

            Image {
                id: icon
                source: "image://icons/" + item.win.icon
                asynchronous: true

                anchors.horizontalCenter: parent.horizontalCenter
                anchors.verticalCenter: parent.verticalCenter
                fillMode: Image.PreserveAspectFit

                height: 48
                width: 48
                sourceSize { width: width; height: height }
            }
        }

        Rectangle {
            id: darken
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.verticalCenter: parent.verticalCenter

            width: box.scaledWinWidth
            height: box.scaledWinHeight

            color: "black"
            opacity: 0.1 * item.darkness
        }
    }

    states: [
        State {
            name: ""
            StateChangeScript {
                name: "activate"
                script: if (item.needsActivation) {
                            item.needsActivation = false;
                            win.active = true;
                            Qt.quit()
                        }
            }
            PropertyChanges {
                target: itemArea
                onClicked: undefined
            }
        },
        State {
            name: "spread"
            PropertyChanges {
                target: item;
                width: columnWidth
                height: (parent.height - parent.anchors.margins) / parent.rows
                x: column * columnWidth
                y: row * height
                darkness: 1.0
            }
            PropertyChanges {
                target: itemArea
                onClicked: { item.z = 1000; item.needsActivation = true; }
            }
            PropertyChanges {
                target: box
                anchors.margins: 8 //FIXME: Check this value in Unity
            }
        }
    ]

    transitions: [
        Transition {
            SequentialAnimation {
                PropertyAction { target: shot; property: "smooth"; value: false }
                NumberAnimation {
                    properties: "x,y,width,height,darkness";
                    duration: 250;
                    easing.type: Easing.InOutSine
                }
                PropertyAction { target: shot; property: "smooth"; value: true }
                ScriptAction { scriptName: "activate" }
            }
        }
    ]
}
