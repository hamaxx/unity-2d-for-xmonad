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

    property int pips: 3
    function getPipShift(index) {
        /* This works and is less convoluted than the generic formula.
           and we're never dealing with more than 3 pips anyway. */
        if (pips == 1) return 0;
        if (pips == 2) return (index == 0) ? -2 : +2
        else return (index == 0) ? 0 : (index == 1) ? -4 : +4
    }

    /* I'd rather use a Column here, but the pip images have an halo
       around them, so they are pretty tall and would mess up the column.
       As a workaround I center all of them, then shift up or down
       depending on the index. The +1 on the translation is be Unity's
       centering is 1px off compared to QML's (due to centering in an
       even-sized parent).
    */
    Item {
        height: 54
        anchors.left: parent.left
        anchors.bottom: parent.bottom

        Repeater {
            model: launcherItem.pips
            delegate: Image {
                source: "image://blended/%1color=%2alpha=%3"
                        .arg(engineBaseUrl +
                             (launcherItem.pips == 1) ? "artwork/launcher_arrow_ltr.png"
                                                      : "artwork/launcher_pip_ltr.png")
                        .arg("lightgrey")
                        .arg(1.0)
                anchors.left: parent.left
                anchors.verticalCenter: parent.verticalCenter

                transform: Translate {
                    y: getPipShift(index) + 1
                }
            }
        }
    }

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
                                            .arg(1.0)
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
        sourceSize.width: 54
        sourceSize.height: 54
    }

}
