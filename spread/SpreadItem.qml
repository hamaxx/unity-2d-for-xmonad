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
    property variant win

    /* Position and size of our cell inside the grid */
    property int column
    property int row
    property real cellWidth
    property real cellHeight

    /* The item's inner margins.
       We are not using QML's anchors.*Margin here to allow
       more flexibility in when to apply margins.
       This is the value for screen mode, which is always zero
       since we don't want to alter the shots at all. */
    property real topMargin: 0
    property real bottomMargin: 0
    property real leftMargin: 0
    property real rightMargin: 0

    /* Emitted when clicking on an item, to tell the parent to
       initiate the outro animation */
    signal speadShouldEnd

    /* Emitted when the outro animation for this item is done */
    signal outroFinished

    /* Only here to make following calculations more readable.
       Assigned only once so not a big performance issue */
    property real widthScale: cellWidth / win.size.width
    property real heightScale: cellHeight / win.size.height

    /* Calculate the final size of the shot inside of the cell.
       Essentially first try to scale it so that the larger side will fit
       completely the cell's inner margins, but preserving the aspect ratio
       (the logic is the same as QML's Image.fillMode = Image.PreserveAspectFit).
       However if this size is larger than the original shot, then keep the original.

       NOTE: these values are very important because all children of the item, not
       just the shot, will use them (since they should overlap perfectly).
       These children include the replacement box+icon for failed screenshots, the
       MouseArea receiving the user's clicks and the opaque Rectangle used for
       showing the currently selected item in the grid.
    */
    property real shotWidth
    property real shotHeight
    shotWidth: {
        var scaledWidth = (widthScale <= heightScale) ? width - (leftMargin + rightMargin) : heightScale * win.size.width
        return Math.min(scaledWidth, win.size.width)
    }
    shotHeight: {
        var scaledHeight = (widthScale <= heightScale) ? widthScale * win.size.height : height - (topMargin + bottomMargin)
        return Math.min(scaledHeight, win.size.height)
    }


    /* The shot itself. The actual image is obtained via the WindowImageProvider which
       serves the "image://window/*" source URIs.
       Please note that the screenshot is taken at the moment the source property is
       actually assigned, during component initialization.
       If taking the screenshot fails (for example for minimized windows), then this
       is hidden and the icon box (see "icon_box" below) is shown. */
    Image {
        id: shot

        width: item.shotWidth
        height: item.shotHeight
        anchors.centerIn: parent

        source: "image://window/" + item.win.xid

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

        width: item.shotWidth
        height: item.shotHeight
        anchors.centerIn: parent

        border.width: 1
        border.color: "black"

        visible: (shot.status == Image.Error)

        Image {
            source: "image://icons/" + item.win.icon
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

        width: item.shotWidth
        height: item.shotHeight
        anchors.centerIn: parent

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

                text: win.title
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

        width: item.shotWidth
        height: item.shotHeight
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
            item.win.active = true
            item.speadShouldEnd()
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

                /* In grid mode, we want to be constrained inside our designated cell */
                width: cellWidth
                height: cellHeight
                x: column * cellWidth
                y: row * cellHeight

                /* The logic for the cell's margins (according to Florian's interpretation
                   of Unity's code) is as follows:
                   * Always leave at least 20 pixels between any shot's side and anything else.
                     (anything else means either another shot or the desktop's available space's
                      borders)
                   * If the shot is smaller than that, it's ok to leave more margin space.

                   To respect the first rule, we have to check if our cell is at the outer sides
                   of the grid, and leave 20 pixels on these sides. On the inner sides only 10
                   pixels are necessary since the neighbour will provide the other 10.
                   To respect the second rule, we just rely on the fact that all shots have
                   anchors.centerIn: parent, therefore they will float in the center if smaller.
                */
                leftMargin: (column == 0) ? 20 : 10
                rightMargin: (column == item.parent.columns - 1) ? 20 : 10
                topMargin: (row == 0) ? 20 : 10
                bottomMargin: (row == item.parent.rows - 1) ? 20 : 10
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
                    properties: "x,y,width,height,leftMargin,rightMargin,topMargin,bottomMargin";
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
