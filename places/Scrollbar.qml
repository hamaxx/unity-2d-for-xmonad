import Qt 4.7

/* Scrollbar composed of:
   - a background track; clicking on it triggers page scrolling
   - a draggable slider on top

   Usage:

   Flickable {
       id: flickable
   }

   Scrollbar {
       targetFlickable: flickable
   }
*/
Item {
    id: scrollbar

    property variant targetFlickable

    width: 10

    BorderImage {
        id: background

        anchors.fill: parent
        source: "artwork/scrollbar/background.sci"
        smooth: false

        MouseArea {
            id: scrollMouseArea

            anchors.fill: parent
            acceptedButtons: Qt.LeftButton | Qt.MiddleButton | Qt.RightButton

            onPressed: {
                /* Scroll one page without overshooting */
                var scrollAmount = mouseY > slider.y ? targetFlickable.height : -targetFlickable.height
                var destination = targetFlickable.contentY + scrollAmount
                var clampedDestination = Math.max(0, Math.min(targetFlickable.contentHeight - targetFlickable.height,
                                                              destination))
                scrollAnimation.to = clampedDestination
                scrollAnimation.restart()
            }

            NumberAnimation {
                id: scrollAnimation

                duration: 200
                easing.type: Easing.InOutQuad
                target: targetFlickable
                property: "contentY"
            }
        }
    }

    Item {
        id: slider

        property int minimalHeight: 40

        anchors.left: parent.left
        anchors.right: parent.right

        y: {
            var clampedYPosition = Math.max(0, Math.min(1-targetFlickable.visibleArea.heightRatio,
                                                        targetFlickable.visibleArea.yPosition))
            return clampedYPosition * scrollbar.height
        }
        height: Math.max(minimalHeight, targetFlickable.visibleArea.heightRatio * scrollbar.height)

        Behavior on height {NumberAnimation {duration: 200; easing.type: Easing.InOutQuad}}

        BorderImage {
            anchors.fill: parent
            /* The glow around the slider is 5 pixels wide */
            anchors.margins: -5

            smooth: false

            source: {
                if(dragMouseArea.pressed)
                    return "artwork/scrollbar/slider_pressed.sci"
                else if(dragMouseArea.containsMouse)
                    return "artwork/scrollbar/slider_hovered.sci"
                else
                    return "artwork/scrollbar/slider_default.sci"
            }

            Image {
                id: handle

                anchors.centerIn: parent
                source: {
                    if(dragMouseArea.pressed)
                        return "artwork/scrollbar/handle_pressed.png"
                    else
                        return "artwork/scrollbar/handle_default.png"
                }
                width: sourceSize.width
                height: sourceSize.height
                fillMode: Image.Tile
                smooth: false
            }
        }

        MouseArea {
            id: dragMouseArea

            anchors.fill: parent
            hoverEnabled: true
            acceptedButtons: Qt.LeftButton | Qt.MiddleButton | Qt.RightButton
            drag.target: slider
            drag.axis: Drag.YAxis
            drag.minimumY: 0
            drag.maximumY: scrollbar.height - slider.height

            onPositionChanged: {
                if (drag.active) {
                    targetFlickable.contentY = slider.y * targetFlickable.contentHeight / scrollbar.height
                }
            }
        }
    }
}
