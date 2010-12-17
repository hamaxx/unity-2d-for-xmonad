import Qt 4.7

Item {
    id: item
    property variant win

    property int column
    property int row
    property int columnsInRow

    property real columnWidth
    property real rowHeight

    property real ratio

    x: (win.location.x * ratio) - desktop.x
    y: (win.location.y * ratio) - desktop.y
    width: win.size.width * ratio
    height: win.size.height * ratio
    z: win.z

    property real topMargin: 0
    property real bottomMargin: 0
    property real leftMargin: 0
    property real rightMargin: 0

    signal finished

    property real widthScale: width / win.size.width
    property real heightScale: height / win.size.height
    property real scaledWinWidth: Math.min((widthScale <= heightScale) ? width : heightScale * win.size.width, win.size.width) - (leftMargin + rightMargin)
    property real scaledWinHeight: Math.min((widthScale <= heightScale) ? widthScale * win.size.height : height, win.size.height) - (topMargin + bottomMargin)


    MouseArea {
        id: itemArea
        anchors.centerIn: parent

        width: item.scaledWinWidth
        height: item.scaledWinHeight

        hoverEnabled: true
        enabled: false
    }

    Image {
        id: shot
        source: "image://window/" + item.win.xid
        anchors.centerIn: parent
        smooth: true

        width: item.scaledWinWidth
        height: item.scaledWinHeight

        visible: (status != Image.Error)
    }

    Rectangle {
        id: window_box
        visible: (shot.status == Image.Error)

        width: item.scaledWinWidth
        height: item.scaledWinHeight

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
        width: item.scaledWinWidth
        height: item.scaledWinHeight
        visible: false

        Rectangle {
            id: darken
            anchors.fill:  parent
            color: "black"
            opacity: 0.1
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

    states: [
        State {
            name: ""
            StateChangeScript {
                name: "activate"
                script: {
                    finished()
                }
            }
        },
        State {
            name: "spread"
            PropertyChanges {
                target: item;
                width: columnWidth
                height: rowHeight
                x: column * columnWidth
                y: row * rowHeight
                leftMargin: (column == 0) ? 20 : 10
                rightMargin: (column == item.parent.columns - 1) ? 20 : 10
                topMargin: (row == 0) ? 20 : 10
                bottomMargin: (row == item.parent.rows - 1) ? 20 : 10
            }
            PropertyChanges {
                target: itemArea
                onClicked: {
                    item.z = 1000
                    item.win.active = true
                    item.parent.state = "" // Trigger the exit from the spread state
                }
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
                    target: item
                    properties: "x,y,width,height,leftMargin,rightMargin,topMargin,bottomMargin";
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
