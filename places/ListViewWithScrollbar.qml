import Qt 4.7

Item {
    property alias list: list
    property alias scrollbar: scrollbar

    ListView {
        id: list

        anchors.top: parent.top
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        anchors.right: scrollbar.left
        anchors.rightMargin: 15

        clip: true
        /* FIXME: proper spacing cannot be set because of the hack in Group.qml
           whereby empty groups are still in the list but invisible and of
           height 0.
        */
        //spacing: 31

        orientation: ListView.Vertical

        /* WARNING - HACK - FIXME
           Issue:
           User wise annoying jumps in the list are observable if cacheBuffer is
           set to 0 (which is the default value). States such as 'folded' are
           lost when scrolling a lot.

           Explanation:
           The height of the Group delegate depends on its content. However its
           content is not known until the delegate is instantiated because it
           depends on the number of results displayed by its GridView.

           Resolution:
           We set the cacheBuffer to the biggest possible int in order to make
           sure all delegates are always instantiated.
        */
        cacheBuffer: 2147483647
    }

    Scrollbar {
        id: scrollbar

        anchors.top: parent.top
        anchors.topMargin: 15
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 10
        anchors.right: parent.right

        targetFlickable: list

        /* Hide the scrollbar if there is less than a page of results */
        opacity: targetFlickable.visibleArea.heightRatio < 1.0 ? 1.0 : 0.0
        Behavior on opacity {NumberAnimation {duration: 100}}
    }
}
