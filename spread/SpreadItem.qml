import Qt 4.7

Item {
    id: item
    property alias icon: icon.source
    property alias capture: shot.source
    property real darkness: 0.0

    property int column
    property int row

    property int columnsInRow: (row == parent.rows - 1 && parent.lastRowColumns != 0) ?
                                   parent.lastRowColumns : parent.columns
    property real columnWidth: (parent.width - parent.anchors.margins) / columnsInRow

    property int winX
    property int winY
    property int winWidth
    property int winHeight
    property alias winZ: item.z

    x: winX * parent.ratio
    y: winY * parent.ratio
    width: winWidth * parent.ratio
    height: winHeight * parent.ratio

    Item {
        id: box
        anchors.fill: parent
        anchors.margins: 8  //TODO: check in unity

        property real widthScale: width / winWidth
        property real heightScale: height / winHeight
        property real scaledWinWidth: ((widthScale <= heightScale) ? parent.width - anchors.margins * 2: heightScale * winWidth)
        property real scaledWinHeight: ((widthScale <= heightScale) ? widthScale * winHeight : parent.height - anchors.margins * 2)

        MouseArea {
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
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.verticalCenter: parent.verticalCenter
                fillMode: Image.PreserveAspectFit
                asynchronous: true

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
            name: "spread"
            PropertyChanges {
                target: item;
                width: columnWidth
                height: (parent.height - parent.anchors.margins) / parent.rows
                x: column * columnWidth
                y: row * height
                darkness: 1.0
            }
        }
    ]

    // TODO: check transitions time and type in Unity
    transitions: Transition {
        PropertyAction { target: shot; property: "smooth"; value: false }
        NumberAnimation {
            properties: "x,y,width,height,darkness";
            duration: 550;
            easing.type: Easing.InOutQuad
        }
        PropertyAction { target: shot; property: "smooth"; value: true }
    }
}
