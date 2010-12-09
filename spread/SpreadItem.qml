import Qt 4.7

Item {
    id: item
    property alias icon: icon.source
    property alias capture: shot.source
    property real darkness: 0.0

    property int column
    property int row

    property int win_x
    property int win_y
    property int win_width
    property int win_height
    property alias win_z: item.z

    x: win_x * parent.ratio
    y: win_y * parent.ratio
    width: win_width * parent.ratio
    height: win_height * parent.ratio

    Item {
        id: box
        anchors.fill: parent
        anchors.margins: 8  //TODO: check in unity

        property real widthScale: width / win_width
        property real heightScale: height / win_height
        property real scaledWinWidth: ((widthScale <= heightScale) ? parent.width : heightScale * win_width) - anchors.margins
        property real scaledWinHeight: ((widthScale <= heightScale) ? widthScale * win_height : parent.height) - anchors.margins

        MouseArea {
            anchors.fill: parent
            hoverEnabled: true
            onEntered: if (item.state == "spread") item.darkness = 0.0
            onExited: if (item.state == "spread") item.darkness = 1.0
        }

        Image {
            id: shot
            z: 2
            anchors.fill: parent
            fillMode: Image.PreserveAspectFit

            Rectangle {
                id: darken
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.verticalCenter: parent.verticalCenter

                width: parent.scaledWinWidth
                height: parent.scaledWinHeight

                color: "black"
                opacity: 0.1 * item.darkness

            }

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
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.verticalCenter: parent.verticalCenter
                fillMode: Image.PreserveAspectFit
                asynchronous: true

                height: 48
                width: 48
                sourceSize { width: width; height: height }
            }
        }
    }

    states: [
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
            duration: 550;
            easing.type: Easing.InOutQuad
        }
    }
}
