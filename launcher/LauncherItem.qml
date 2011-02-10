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

    property int pips: 0
    property string pipSource: engineBaseUrl + "artwork/launcher_" +
                               ((pips <= 1) ? "arrow" : "pip") + "_ltr.png"
    function getPipOffset(index) {
        /* This works and is less convoluted than the generic formula.
           and we're never dealing with more than 3 pips anyway. */
        if (pips == 1) return 0;
        if (pips == 2) return (index == 0) ? -2 : +2
        else return (index == 0) ? 0 : (index == 1) ? -4 : +4
    }

    signal clicked(variant mouse)
    signal entered
    signal exited

    Item {
        height: 54
        width: parent.width
        anchors.bottom: parent.bottom

        Image {
            anchors.right: parent.right
            anchors.verticalCenter: parent.verticalCenter
            source: "image://blended/%1color=%2alpha=%3"
                  .arg(engineBaseUrl + "artwork/launcher_arrow_rtl.png")
                  .arg("lightgrey")
                  .arg(1.0)

            /* This extra shift is necessary (as is for the pips below)
               since we are vertically centering in a parent with even height, so
               there's one arbitrary pixel that need to be assigned arbitrarily.
               Unity chose to add it, QML to subtract it. So we adjust for that. */
            transform: Translate {
                y: 1
            }

            visible: active
        }

        /* I'd rather use a Column here, but the pip images have an halo
           around them, so they are pretty tall and would mess up the column.
           As a workaround I center all of them, then shift up or down
           depending on the index. */
        Repeater {
            model: launcherItem.pips
            delegate: Image {
                source: "image://blended/%1color=%2alpha=%3"
                        .arg(pipSource).arg("lightgrey").arg(1.0)
                anchors.left: parent.left
                anchors.verticalCenter: parent.verticalCenter

                transform: Translate {
                    y: getPipOffset(index) + 1
                }
            }
        }

        MouseArea {
            id: mouse

            acceptedButtons: Qt.LeftButton | Qt.RightButton
            hoverEnabled: true
            anchors.fill: parent
            onClicked: launcherItem.clicked(mouse)
            onEntered: launcherItem.entered()
            onExited: launcherItem.exited()
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
