import Qt 4.7
import Unity2d 1.0

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
    id: launcherItem

    property alias icon: icon.source
    //property alias label: label.text
    property bool running: false
    property bool active: false
    property bool urgent: false
    property bool launching: false

    property real backlight: 0.9

    Image {
        id: tileBackground
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottom: parent.bottom
        width: 54
        height: 54

        sourceSize.width: 54
        sourceSize.height: 54

        opacity: 0.75
    }

    Image {
        id: tileOutline
        width: 54
        height: 54
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottom: parent.bottom

        source: "artwork/round_outline_54x54.png"
//        source: "image://blended/%1color=%2alpha=%3".arg(engineBaseUrl + "artwork/round_outline_54x54.png")
//                                                    .arg("AAAAAA")
//                                                    .arg(1.0 - launcherItem.backlight)
        sourceSize.width: 54
        sourceSize.height: 54

        opacity: 0.75
    }

    Item {
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottom: parent.bottom
        width: 54
        height: 54

        Image {
            id: icon
            anchors.centerIn: parent

            sourceSize.width: 48
            sourceSize.height: 48

            onWidthChanged: updateColors()
            onHeightChanged: updateColors()
            onSourceChanged: updateColors()

            function updateColors() {
                var colors = launcherView.getColorsFromIcon(icon.source, icon.sourceSize)
                if (colors) {
                    tileBackground.source = "image://blended/%1color=%2alpha=%3"
                                            .arg(engineBaseUrl + "artwork/round_corner_54x54.png")
                                            .arg(colors[0].toString().replace("#", ""))
                                            .arg(1.0) //launcherItem.backlight)
                }
            }
        }
    }


    Image {
        id: tileShine
        width: 54
        height: 54
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottom: parent.bottom

        source: "artwork/round_shine_54x54.png"
//        source: "image://blended/%1color=%2alpha=%3".arg(engineBaseUrl + "artwork/round_shine_54x54.png")
//                                                    .arg("white")
//                                                    .arg(launcherItem.backlight)
        sourceSize.width: 54
        sourceSize.height: 54
    }

}
