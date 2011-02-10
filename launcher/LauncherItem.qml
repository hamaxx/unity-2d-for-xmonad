import Qt 4.7
import Unity2d 1.0

/* Item displaying a launcher item.

   The 'icon' property holds the source of the image to load as an icon.
   The 'label' property holds the text to display.
   The 'running' property is a boolean indicating whether or not the
   application is launched. When the application is launched, its 'windowCount'
   property will have a value reflecting the number of open windows.
*/
Item {
    id: launcherItem
    anchors.horizontalCenter: parent.horizontalCenter

    property alias icon: icon.source
    property bool running: false
    property bool active: false
    property bool urgent: false
    property bool launching: false

    /* Pips are small icons used to indicate how many windows we have open for
       the current tile. If there's only one, we just display an arrow, if there are
       two we display 2 pips, if there are 3 or more we display 3 pips */
    property int pips: 0
    property string pipSource: engineBaseUrl + "artwork/launcher_" +
                               ((pips <= 1) ? "arrow" : "pip") + "_ltr.png"
    function getPipOffset(index) {
        /* Pips need to always be centered, regardless if they are an even or odd
           number. The following simple conditional code works and is less
           convoluted than a generic formula. It's ok since we always work with at
           most three pips anyway. */
        if (pips == 1) return 0;
        if (pips == 2) return (index == 0) ? -2 : +2
        else return (index == 0) ? 0 : (index == 1) ? -4 : +4
    }

    signal clicked(variant mouse)
    signal entered
    signal exited

    Image {
        anchors.right: parent.right
        anchors.verticalCenter: parent.verticalCenter
        source: "image://blended/%1color=%2alpha=%3"
              .arg(engineBaseUrl + "artwork/launcher_arrow_rtl.png")
              .arg("lightgrey")
              .arg(1.0)

        /* This extra shift is necessary (as is for the pips below)
           since we are vertically centering in a parent with even height, so
           there's one pixel offset that need to be assigned arbitrarily.
           Unity chose to add it, QML to subtract it. So we adjust for that. */
        transform: Translate { y: 1 }

        visible: active
    }

    /* I'd rather use a Column here, but the pip images have an halo
       around them, so they are pretty tall and would mess up the column.
       As a workaround I center all of them, then shift up or down
       depending on the index. */
    Repeater {
        model: launcherItem.pips
        delegate: Image {
            anchors.left: parent.left
            anchors.verticalCenter: parent.verticalCenter
            source: "image://blended/%1color=%2alpha=%3"
                    .arg(pipSource).arg("lightgrey").arg(1.0)

            transform: Translate { y: getPipOffset(index) + 1 }
        }
    }

    /* Container for centering the actual tile */
    Item {
        anchors.centerIn: parent
        width: 54
        height: parent.height

        Image {
            id: tileBackground
            anchors.fill: parent

            sourceSize.width: 54
            sourceSize.height: 54

            opacity: 0.75
        }

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

        Image {
            id: tileShine
            anchors.fill: parent

            source: "artwork/round_shine_54x54.png"
            sourceSize.width: 54
            sourceSize.height: 54
        }
    }

    MouseArea {
        id: mouse
        anchors.fill: parent

        hoverEnabled: true
        acceptedButtons: Qt.LeftButton | Qt.RightButton
        onClicked: launcherItem.clicked(mouse)
        onEntered: launcherItem.entered()
        onExited: launcherItem.exited()
    }
}
