import Qt 4.7
import Effects 1.0

AbstractButton {
    id: groupHeader

    property string icon
    property alias label: title.text
    property bool folded: true
    property int availableCount

    onClicked: folded = !folded

    effect: DropShadow {
                blurRadius: 6
                offset.x: 0
                offset.y: 0
                color: "white"
                enabled: groupHeader.state == "pressed"
            }

    Image {
        id: iconImage

        source: icon

        width: sourceSize.width
        height: sourceSize.height
        anchors.bottom: underline.top
        anchors.bottomMargin: 5
        anchors.left: parent.left
        anchors.leftMargin: 8
        fillMode: Image.PreserveAspectFit
    }

    TextCustom {
        id: title

        font.pixelSize: 16
        anchors.baseline: underline.top
        anchors.baselineOffset: -10
        anchors.left: iconImage.right
        anchors.leftMargin: 8
    }

    Item {
        id: moreResults

        visible: availableCount > 0
        anchors.left: title.right
        anchors.leftMargin: 11
        anchors.baseline: title.baseline

        opacity: groupHeader.state == "selected" || groupHeader.state == "pressed" ? 1.0 : 0.5
        Behavior on opacity {NumberAnimation { duration: 100 }}

        effect: DropShadow {
                    blurRadius: 4
                    offset.x: 0
                    offset.y: 0
                    color: "white"
                    enabled: moreResults.opacity == 1.0
                }

        TextCustom {
            id: label

            text: groupHeader.folded ? qsTr("See %1 more results").arg(availableCount) : qsTr("See fewer results")
            anchors.left: parent.left
            anchors.baseline: parent.baseline
        }

        FoldingArrow {
            id: arrow

            folded: groupHeader.folded
            anchors.left: label.right
            anchors.leftMargin: 10
            anchors.verticalCenter: label.verticalCenter
        }
    }

    Rectangle {
        id: underline

        color: "white"
        opacity: groupHeader.state == "pressed" ? 0.45 : 0.3

        height: 1
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        anchors.right: parent.right
    }
}
