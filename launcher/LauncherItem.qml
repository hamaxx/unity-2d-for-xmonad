import Qt 4.7
import Unity2d 1.0

/* This component represents a single "tile" in the launcher and the surrounding
   indicator icons.

   The tile is square in size, with a side determined by the 'tileSize' property,
   and rounded borders.
   It is composed by a colored background layer, an icon (with 'icon' as source),
   and a layer on top that provides a "shine" effect.
   The main color of the background layer may be calculated based on the icon color
   or may be fixed (depending on the 'backgroundFromIcon' property).

   There's also an additional layer which contains only the outline of the tile
   that is only appearing during the launching animation (when the 'launching' property is
   true). During this animation the background fades out and the outline fades in,
   giving a "pulsing" appearance to the tile.

   Around the tile we may have on the left a number of "pips" between zero and three.
   Pips are small icons used to indicate how many windows we have open for the current tile
   (based on the 'windowCount' property).
   The rule is: if there's only one window, we just display an arrow. If there are
   two we display 2 pips, if there are 3 or more display 3 pips.

   On the right of the tile there's an arrow that appears if the tile is currently 'active'.

   Additionally, when the tile is marked as 'urgent' it will start an animation where the
   rotation is changed so that it appears to be "shaking".
*/
Item {
    id: item
    /* The object name is used by the launcher view to find the current
       launcher item under the mouse cursor during a drag’n’drop event. */
    objectName: "launcherItem"

    anchors.horizontalCenter: parent.horizontalCenter
    /* Manually add some padding to compensate for the spacing
       of the ListView being set to 0 to work around
       http://bugreports.qt.nokia.com/browse/QTBUG-17622. */
    property int padding: 5
    height: tileSize + padding

    property int tileSize
    property string desktopFile: ""
    property alias icon: icon.source
    property bool running: false
    property bool active: false
    property bool urgent: false
    property bool launching: false

    property int counter: 0
    property bool counterVisible: false
    property real progress: 0.0
    property bool progressBarVisible: false
    property alias emblem: emblemIcon.source
    property bool emblemVisible: false

    property bool backgroundFromIcon
    property color defaultBackgroundColor: "#333333"

    property alias shortcutVisible: shortcut.visible
    property alias shortcutText: shortcutText.text

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

    Item {
        /* The actual item, reparented so its y coordinate can be animated. */
        id: looseItem
        parent: launcher
        width: item.width
        height: item.height
        x: item.x
        /* item.parent is the delegate, and its parent is the LauncherList */
        y: item.parent.parent.y - item.parent.parent.contentY + item.y
        z: item.parent.parent.itemZ

        /* Bind to the scale of the delegate so that it is animated upon insertion/removal */
        scale: item.scale

        /* This is the arrow shown at the right of the tile when the application is
           the active one */
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

            visible: active && (looseItem.state != "beingDragged")
        }

        /* This is the area on the left of the tile where the pips/arrow end up.

           I'd rather use a Column here, but the pip images have an halo
           around them, so they are pretty tall and would mess up the column.
           As a workaround I center all of them, then shift up or down
           depending on the index. */
        Repeater {
            model: item.pips
            delegate: Image {
                /* FIXME: It seems that when the image is created (or re-used) by the Repeater
                   for a moment it doesn't have any parent, and therefore warnings are
                   printed for the following two anchor assignements. This fixes the
                   problem, but I'm not sure if it should happen in the first place. */
                anchors.left: (parent) ? parent.left : undefined
                anchors.verticalCenter: (parent) ? parent.verticalCenter : undefined

                source: "image://blended/%1color=%2alpha=%3"
                        .arg(pipSource).arg("lightgrey").arg(1.0)

                transform: Translate { y: getPipOffset(index) + 1 }

                visible: looseItem.state != "beingDragged"
            }
        }

        /* This is the for centering the actual tile in the launcher */
        Item {
            id: tile
            width: item.tileSize
            height: item.tileSize
            anchors.centerIn: parent

            /* This is the image providing the background image. The
               color blended with this image is obtained from the color of the icon when it's
               loaded.
               While the application is launching, this will fade out and in. */
            Image {
                id: tileBackground
                property color color: defaultBackgroundColor
                anchors.fill: parent

                SequentialAnimation on opacity {
                    NumberAnimation { to: 0.0; duration: 1000; easing.type: Easing.InOutQuad }
                    NumberAnimation { to: 1.0; duration: 1000; easing.type: Easing.InOutQuad }

                    loops: Animation.Infinite
                    alwaysRunToEnd: true
                    running: launching
                }

                sourceSize.width: item.tileSize
                sourceSize.height: item.tileSize
                source: "image://blended/%1color=%2alpha=%3"
                        .arg(engineBaseUrl + "artwork/round_corner_54x54.png")
                        .arg(color.toString().replace("#", ""))
                        .arg(1.0)
            }

            /* This image appears only while launching, and pulses in and out in counterpoint
               to the background, so that the outline of the tile is always visible. */
            Image {
                id: tileOutline
                anchors.fill: parent

                sourceSize.width: item.tileSize
                sourceSize.height: item.tileSize
                source: "artwork/round_outline_54x54.png"

                opacity: 0

                SequentialAnimation on opacity {
                    NumberAnimation { to: 1.0; duration: 1000; easing.type: Easing.InOutQuad }
                    NumberAnimation { to: 0.0; duration: 1000; easing.type: Easing.InOutQuad }

                    loops: Animation.Infinite
                    alwaysRunToEnd: true
                    running: launching
                }
            }

            /* This is just the main icon of the tile */
            Image {
                id: icon
                anchors.centerIn: parent

                sourceSize.width: 48
                sourceSize.height: 48

                /* Whenever one of the parameters used in calculating the background color of
                   the icon changes, recalculate its value */
                onWidthChanged: updateColors()
                onHeightChanged: updateColors()
                onSourceChanged: updateColors()

                function updateColors() {
                    if (!item.backgroundFromIcon) return;

                    var colors = launcherView.getColorsFromIcon(icon.source, icon.sourceSize)
                    if (colors && colors.length > 0) tileBackground.color = colors[0]
                }
            }

            /* This just adds some shiny effect to the tile */
            Image {
                id: tileShine
                anchors.fill: parent

                source: "artwork/round_shine_54x54.png"
                sourceSize.width: item.tileSize
                sourceSize.height: item.tileSize
            }

            Rectangle {
                id: counter
                height: 16 - border.width
                width: 32
                // Using anchors the item will be 1 pixel off with respect to Unity
                y: 1
                x: 1
                radius: height / 2 - 1
                border.width: 2
                border.color: "white"
                color: "#595959"
                visible: launcherItem.counterVisible

                Text {
                    anchors.centerIn: parent
                    font.pixelSize: parent.height - 3
                    width: parent.width - 5
                    elide: Text.ElideRight
                    horizontalAlignment: Text.AlignHCenter
                    color: "white"
                    text: launcherItem.counter
                }
            }

            Image {
                id: progressBar
                source: "artwork/progress_bar_trough.png"
                anchors.verticalCenter: parent.verticalCenter
                anchors.left: parent.left
                width: tile.width
                state: launcherItem.progressBarVisible ? "" : "hidden"

                Image {
                    id: progressFill
                    source: "artwork/progress_bar_fill.png"
                    anchors.verticalCenter: parent.verticalCenter
                    x: 6
                    width: sourceSize.width * launcherItem.progress

                    Behavior on width {
                       NumberAnimation { duration: 200; easing.type: Easing.InOutSine }
                    }
                }

                Behavior on width {
                    NumberAnimation { duration: 200; easing.type: Easing.InOutSine }
                }

                states: State {
                    name: "hidden"
                    PropertyChanges {
                        target: progressBar
                        width: 0
                    }
                    // This, combined with anchors.left: parent.left in the default state
                    // makes the bar seem to come in from the left and go away at the right
                    AnchorChanges {
                        target: progressBar
                        anchors.left: undefined
                        anchors.right: tile.right
                    }
                }
            }

            Rectangle {
                id: shortcut
                anchors.centerIn: parent
                color: "#B3000000" // 0.7 opacity on black
                radius: 2
                width: 22
                height: 22

                Text {
                    id: shortcutText
                    anchors.centerIn: parent
                    color: "white"
                }
            }

            Image {
                id: emblemIcon
                anchors.left: parent.left
                anchors.top: parent.top
                visible: launcherItem.emblemVisible && !counter.visible
            }


            /* The entire tile will "shake" when the window is marked as "urgent", to attract
               the user's attention */
            SequentialAnimation {
                running: urgent
                alwaysRunToEnd: true

                SequentialAnimation {
                    loops: 30
                    NumberAnimation { target: tile; property: "rotation"; to: 15; duration: 150 }
                    NumberAnimation { target: tile; property: "rotation"; to: -15; duration: 150 }
                }
                NumberAnimation { target: tile; property: "rotation"; to: 0; duration: 75 }
            }

            MouseArea {
                id: mouse
                anchors.fill: parent

                hoverEnabled: true
                acceptedButtons: Qt.LeftButton | Qt.RightButton
                onClicked: item.clicked(mouse)
                onEntered: item.entered()
                onExited: item.exited()
            }
        }

        states: State {
            name: "beingDragged"
            when: (dnd.currentId != "") && (dnd.currentId == item.desktopFile)
            PropertyChanges {
                target: looseItem
                /* item.parent is the delegate, and its parent is the LauncherList */
                y: dnd.listCoordinates.y - item.parent.parent.contentY - tile.height / 2
                /* When dragging an item, stack it on top of all its siblings */
                z: 1
            }
        }
        Behavior on y {
            enabled: (looseItem.state != "beingDragged") && !item.parent.parent.moving && !item.parent.parent.autoScrolling
            NumberAnimation {
                duration: 400
                easing.type: Easing.OutBack
            }
        }
    }
}
