import QtQuick 1.0

Item {
    id: background

    property string state

    opacity: ( state == "selected" || state == "pressed"
              || state == "hovered" ) ? 1.0 : 0.0
    Behavior on opacity {NumberAnimation {duration: 100}}

    Rectangle {

        /* FIXME: */
        anchors.fill: parent
        anchors.bottomMargin: 1
        anchors.rightMargin: 1

        color: background.state == "pressed" ? "#ffffffff" : "#00000000"
        border.color: "#cccccc"
        border.width: 1
        radius: 3

        Image {
            fillMode: Image.Tile
            anchors.fill: parent
            source: "artwork/button_background.png"
            smooth: false
        }
    }
}
