import Qt 4.7

Item {
    id: item
    property alias appName: appName.text
    property alias icon: img.source
    property alias title: title.text
    property alias capture: shot.source
    property real darkness: 0.0

    property int column
    property int row

    property int win_x
    property int win_y
    property int win_width
    property int win_height
    property alias win_z: item.z

    Rectangle {
        anchors.fill: parent
        anchors.margins: 8  //TODO: check in unity
        color: "white"
        border.width: 2

        MouseArea {
            anchors.fill: parent
            hoverEnabled: true
            onEntered: if (item.state == "spread") item.darkness = 0.0
            onExited: if (item.state == "spread") item.darkness = 1.0
        }

        Image {
            id: shot
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.verticalCenter: parent.verticalCenter
            width: parent.width
            height: parent.height
        }

        Text {
            id: appName
            anchors.top: parent.top
            anchors.left: parent.left
            anchors.right: parent.right

            horizontalAlignment: Text.AlignHCenter
            elide: Text.ElideMiddle

            Rectangle {
                anchors.fill: parent
                color: "yellow"
                z: -1
            }
        }

        Image {
            id: img
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.verticalCenter: parent.verticalCenter
            width: 48
            height: 48
        }

        Text {
            id: title
            anchors.bottom: parent.bottom
            anchors.left: parent.left
            anchors.right: parent.right

            horizontalAlignment: Text.AlignHCenter
            elide: Text.ElideRight

            Rectangle {
                anchors.fill: parent
                color: "orange"
                z: -1
            }
        }

        Rectangle {
            id: darken
            anchors.fill: parent
            color: "black"
            opacity: 0.1 * item.darkness
        }
    }

    states: [
        State {
            name: "screen"
            PropertyChanges {
                target: item;
                x: win_x * item.parent.ratio
                y: win_y * item.parent.ratio
                width: win_width * item.parent.ratio
                height: win_height * item.parent.ratio
                darkness: 0.0
            }
        },
        State {
            name: "spread"
            PropertyChanges {
                target: item;
                y: height * row
                x: {
                    if (!parent.full && row == item.parent.rows - 1) {
                        var gap = parent.rows * parent.columns - parent.count
                        width * column + (gap * width / 2.0)
                    }
                    else
                        width * column
                }
                width: (parent.width - parent.anchors.margins) / parent.columns
                height: (parent.height - parent.anchors.margins) / parent.rows
                darkness: 1.0
            }
        }
    ]

    // TODO: check transitions time and type in Unity
    transitions: Transition {
        NumberAnimation {
            properties: "x,y,width,height,darkness";
            duration: 500;
            easing.type: Easing.InOutQuad
        }
    }
}
