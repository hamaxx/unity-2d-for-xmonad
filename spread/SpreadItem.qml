import Qt 4.7

Item {
    id: item
    property variant win

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

    property real darkness: 0.0

    Item {
        id: box
        anchors.fill: parent

        property real widthScale: width / item.win.size.width
        property real heightScale: height / item.win.size.height
        property real scaledWinWidth: ((widthScale <= heightScale) ? parent.width - anchors.margins * 2: heightScale * item.win.size.width)
        property real scaledWinHeight: ((widthScale <= heightScale) ? widthScale * item.win.size.height : parent.height - anchors.margins * 2)

        MouseArea {
            id: itemArea
            anchors.centerIn: parent

            width: parent.scaledWinWidth
            height: parent.scaledWinHeight

            hoverEnabled: true
            enabled: false
        }

        Image {
            id: shot
            source: "image://window/" + item.win.xid
            anchors.fill: parent
            fillMode: Image.PreserveAspectFit
            smooth: true

            visible: (status != Image.Error)
        }

        Rectangle {
            id: window_box
            visible: (shot.status == Image.Error)

            width: parent.scaledWinWidth
            height: parent.scaledWinHeight

            anchors.centerIn: parent

            border.width: 1
            border.color: "black"

            Image {
                id: icon
                source: "image://icons/" + item.win.icon
                asynchronous: true

                anchors.centerIn: parent
                fillMode: Image.PreserveAspectFit

                height: 48
                width: 48
                sourceSize { width: width; height: height }
            }
        }

        Item {
            id: itemExtras

            anchors.centerIn: parent
            width: box.scaledWinWidth
            height: box.scaledWinHeight
            visible: false

            Rectangle {
                id: darken
                anchors.fill:  parent
                color: "black"
                opacity: 0.1 * item.darkness
            }

            Rectangle {
                id: labelBox

                /* This property (and the weird logic that uses it below)
                   is needed since using regular margins isn't possible while
                   scaling the text to the size of the outer box wihtout having
                   a bindings loop */
                property int labelMargins: 6

                anchors.centerIn: parent
                width: Math.min(parent.width, label.paintedWidth + labelMargins)
                height: label.height + labelMargins
                visible: false

                radius: 3
                color: "#99000000" // should equal: backgroundColor: "black"; opacity: 0.6

                Text {
                    id: label
                    text: win.title
                    elide: Text.ElideRight
                    color: "white"

                    anchors.centerIn: parent
                    width: itemExtras.width - parent.labelMargins
                    horizontalAlignment: Text.AlignHCenter
                }
            }

            states: State {
                when: itemArea.containsMouse
                PropertyChanges { target: darken; visible: false }
                PropertyChanges { target: labelBox; visible: true }
            }
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
                PropertyAction { target: itemArea; property: "enabled"; value: false }
                PropertyAction { target: itemExtras; property: "visible"; value: false }
                NumberAnimation {
                    properties: "x,y,width,height";
                    duration: 250;
                    easing.type: Easing.InOutSine
                }
                PropertyAction { target: shot; property: "smooth"; value: true }
                PropertyAction { target: itemArea; property: "enabled"; value: true }
                PropertyAction { target: itemExtras; property: "visible"; value: true }
                ScriptAction { scriptName: "activate" }
            }
        }
    ]
}
