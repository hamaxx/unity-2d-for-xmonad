import Qt 4.7

/*
Each SpreadItem represents a real window on the desktop (we are
passed a WindowInfo object with all the information about it).

Its state ("" or "spread") decides which mode the item should
follow to position itself on screen ("screen" or "spread" mode
respectively). The state is the same as state of the whole grid.

In screen mode we does just use the real window's position and size
to exactly mimic it.

In grid mode, we are assigned a cell in the grid, and we resize
and reposition ourselves to be fully constrained and centered inside
that specific cell.
The shot should occupy as much space as possible inside the cell,
but never be bigger than its original window's size, and always
maintain the same aspect ratio as the original window.

Please note that a big chunk of the size calculations are taking place
in the states property, at the bottom of this file.
*/

Item {
    id: item
    property variant windowInfo

    /* Position and size of our cell inside the grid */
    property int column
    property int row
    property real cellWidth
    property real cellHeight

    /* Values applied when in 'spread' mode */
    property int minMargin: 20
    property int availableWidth: cellWidth-minMargin
    property int availableHeight: cellHeight-minMargin
    /* Scale down to fit availableWidth/availableHeight while preserving the aspect
       ratio of the window. Never scale up the window. */
    property bool isHorizontal: windowInfo.size.width-availableWidth >= windowInfo.size.height-availableHeight
    property int maxWidth: Math.min(windowInfo.size.width, availableWidth)
    property int maxHeight: Math.min(windowInfo.size.height, availableHeight)
    property int spreadWidth: isHorizontal ? maxWidth : windowInfo.size.width * maxHeight / windowInfo.size.height
    property int spreadHeight: !isHorizontal ? maxHeight : windowInfo.size.height * maxWidth / windowInfo.size.width
    /* Center item within its cell */
    property int spreadX: column * cellWidth + (cellWidth-spreadWidth)/2
    property int spreadY: row * cellHeight + (cellHeight-spreadHeight)/2

    /* Emitted when clicking on an item, to tell the parent to
       initiate the outro animation */
    signal spreadShouldEnd

    /* Emitted when the outro animation for this item is done */
    signal outroFinished

    /* The shot itself. The actual image is obtained via the WindowImageProvider which
       serves the "image://window/*" source URIs.
       Please note that the screenshot is taken at the moment the source property is
       actually assigned, during component initialization.
       If taking the screenshot fails (for example for minimized windows), then this
       is hidden and the icon box (see "icon_box" below) is shown. */
    Image {
        id: shot

        anchors.fill: parent
        fillMode: Image.Stretch

        source: "image://window/" + windowInfo.xid

        /* This will be disabled during intro/outro animations for performance reasons,
           but it's good to have in grid mode when the window is */
        smooth: true

        visible: (status != Image.Error)
    }

    /* This replaces the shot whenever retrieving its image fails.
       It is essentially a white rectangle of the same size as the the shot would,
       with a border and the icon for the window floating in the center.
    */
    Rectangle {
        id: iconBox

        anchors.fill: parent

        border.width: 1
        border.color: "black"

        visible: (shot.status == Image.Error)

        Image {
            source: "image://icons/" + windowInfo.icon
            asynchronous: true

            anchors.centerIn: parent
            fillMode: Image.PreserveAspectFit

            /* Please note that sourceSize is necessary, otherwise the
               IconImageProvider will crash when loading the icon */
            height: 48
            width: 48
            sourceSize { width: width; height: height }
        }
    }

    /* This grouping item is necessary to calculate properly the size of
       the label. See below */
    Item {
        id: itemExtras

        anchors.fill: parent

        /* Shown only in grid mode, see transitions */
        visible: false

        /* A darkened rectangle that by default covers all shots
           in grid mode, except the currently selected item. See itemExtras.states */
        Rectangle {
            id: darken

            anchors.fill:  parent

            color: "black"
            opacity: 0.1
        }

        /* A label with the window title centered over the shot.
           It will appear only for the currently selected item. See itemExtras.states */
        Rectangle {
            id: labelBox

            /* The width of the box around the text should be the same as
               the text itself, with 3 pixels of margin on all sides, but it should also
               never overflow the shot's borders.

               Normally one would just set anchors.margins, but this can't work
               here because first we need to let the Text calculate it's "natural" width
               ("paintedWidth" in QT terms) -- that is, the size it would have
               ìf free to expand horizontally unconstrained, to know if it's smaller than
               the labelBox or not.
               However if we bind the Text's width to the width of the labelBox, and the
               width of the labelBox to the Text's size, we get a binding loop error.

               The trick is to bind the Text's width to the labelBox's parent, and then
               the labelBox to the Text's size. Since the relation between labelBox and
               parent is taken care of by the positioner indirectly, there's no loop.

               Yeah, messy. Blame QML ;)
            */
            property int labelMargins: 6
            width: Math.min(parent.width, label.paintedWidth + labelMargins)
            height: label.height + labelMargins
            anchors.centerIn: parent

            /* This equals backgroundColor: "black" and opacity: 0.6
               but we don't want to set it that way since it would be
               inherited by the Text child, and we want it to be fully
               opaque instead */
            color: "#99000000"
            radius: 3
            visible: false

            Text {
                id: label

                anchors.centerIn: parent
                width: itemExtras.width - parent.labelMargins

                text: windowInfo.title
                elide: Text.ElideRight
                horizontalAlignment: Text.AlignHCenter

                color: "white"
            }
        }

        /* If we are hovering this item, show the label and hide the dark box.
           The opposite should happen when we're not hovering the item */
        states: State {
            when: itemArea.containsMouse
            PropertyChanges { target: darken; visible: false }
            PropertyChanges { target: labelBox; visible: true }
        }
    }

    /* This covers the entire shot (or iconBox) area, and serve two
       functions: change the currently selected item by mouseOver, and
       trigger the outro sequence when clicking on an item */
    MouseArea {
        id: itemArea

        width: shot.paintedWidth
        height: shot.paintedHeight
        anchors.centerIn: parent

        /* Since it should be possible to interact with items only
           in grid mode, no events are handled except when enabled
           explicity (see item.transitions) */
        enabled: false
        hoverEnabled: true

        onClicked: {
            /* This is a bit of an hack, to raise the item above all the others
               before starting the outro, but it's ok since at the end of the outro
               all the items will be unloaded (and we will never have >9999 items) */
            item.z = 9999
            /* Immediately activate the window. Since spread is an
               always-on-top window, we can raise the window now so it
               will be already in the correct position when the outro finishes */
            windowInfo.active = true
            item.spreadShouldEnd()
        }
    }

    states: [
        /* This is the default state. Since items are created in this state, we don't
           set any of their values here. See how the default properties are assigned in
           SpreadGrid and SpreadItem.
           At the end of the outro, items will return to this state, and their properties
           to their default values (i.e. the correct values for screen mode). */
        State {
            name: ""

            /* See the transitions are for an explanation of this */
            StateChangeScript {
                name: "endOfOutro"
                script: item.outroFinished()
            }
        },

        /* This state is what we want to have at the end of the intro.
           In other words, it puts the item in its right place and size when in grid mode. */
        State {
            name: "spread"
            PropertyChanges {
                target: item;

                width: spreadWidth
                height: spreadHeight
                x: spreadX
                y: spreadY
            }
        }
    ]

    transitions: [
        /* This is the animation that is exectuted when moving between any of the two states.
           It will be executed in the same sequence for both the intro and outro. */
        Transition {
            SequentialAnimation {
               PropertyAction { target: shot; property: "smooth"; value: false }
                PropertyAction { target: itemArea; property: "enabled"; value: false }
                PropertyAction { target: itemExtras; property: "visible"; value: false }
                NumberAnimation {
                    target: item
                    properties: "x,y,width,height";
                    duration: 250;
                    easing.type: Easing.InOutSine
                }
                PropertyAction { target: shot; property: "smooth"; value: true }
                PropertyAction { target: itemArea; property: "enabled"; value: (item.state == "spread") }
                PropertyAction { target: itemExtras; property: "visible"; value: true }
                ScriptAction { scriptName: "endOfOutro" }
            }
        }
    ]
}
