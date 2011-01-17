import Qt 4.7
import UnityApplications 1.0 /* Necessary for the ImageProvider serving image://icons */
import UnityPlaces 1.0

/* Renderers typically use a grid layout to render the model. The RendererGrid
   component provides a standard implementation of such a layout where the
   cells can be customized by providing a QML component to it.
   A user of RendererGrid would create a renderer inheriting from it
   and pass a Component via the 'cellRenderer' property.
*/
Renderer {
    id: renderer

    property variant cellRenderer
    property bool folded
    folded: {
        /* Look for the groupId as a complete word inside the list of expanded groups.
           Examples of ExpandedGroups hint: "2", "1 3 7", "1 2", etc.
         */
        var re = RegExp("\\b%1\\b".arg(renderer.groupId))
        var expandedGroups = placeEntryModel.entryRendererHints["ExpandedGroups"]
        return !re.test(expandedGroups)
    }

    property int cellWidth: 158
    property int cellHeight: 76
    property int horizontalSpacing: 26
    property int verticalSpacing: 26

    /* Using results.contentHeight produces binding loop warnings and potential
       rendering issues. We compute the height manually.
    */
    /* FIXME: tricking the system by making the delegate of height 0 and with
              an invisible header is no good: the item in the model still
              exists and some things such as keyboard selection break.
    */
    height: results.count > 0 ? header.height + results_layout.anchors.topMargin + results.totalHeight : 0
    //Behavior on height {NumberAnimation {duration: 200}}

    GroupHeader {
        id: header

        visible: results.count > 0
        availableCount: results.count - results.cellsPerLine
        folded: parent.folded
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        height: 28
        icon: parent.iconHint
        label: parent.displayName

        onClicked: parent.folded = !parent.folded
    }

    Item {
        id: results_layout

        anchors.top: header.bottom
        anchors.topMargin: 14
        anchors.left: parent.left
        anchors.leftMargin: 2
        anchors.right: parent.right
        anchors.bottom: parent.bottom

        GridView {
            id: results

            /* FIXME: this is a gross hack compensating for the lack of sections
               in GridView (see ListView.section).

               We nest GridViews inside a ListView and add headers manually
               (GroupHeader). The total height of each Group is computed
               manually and given back to the ListView. However that size cannot
               be used by the individual GridViews because it would make them
               load all of their delegates at once using far too much memory and
               processing power. Instead we constrain the height of the GridViews
               and compute their position manually to compensate for the position
               changes when flicking the ListView.

               We assume that renderer.parentListView is the ListView we nest our
               GridView into.
            */
            property variant flickable: renderer.parentListView

            /* flickable.contentY*0 is equal to 0 but is necessary in order to
               have the entire expression being evaluated at the right moment.
            */
            property int inFlickableY: flickable.contentY*0+parent.mapToItem(flickable, 0, 0).y
            property int compensateY: inFlickableY > 0 ? 0 : -inFlickableY

            width: flickable.width
            height: Math.min(totalHeight, flickable.height)
            onCompensateYChanged: {
                if (flickable.height > 0 && totalHeight >= flickable.height) {
                    y = compensateY
                    contentY = compensateY
                }
            }

            property int cellsPerLine: Math.floor(width/results.cellWidth)
            /* Only display one line of items when folded */
            property int displayedCount: folded ? cellsPerLine : count
            property int totalHeight: results.cellHeight*Math.ceil(displayedCount/cellsPerLine)

            cellWidth: renderer.cellWidth+renderer.horizontalSpacing
            cellHeight: renderer.cellHeight+renderer.verticalSpacing

            interactive: false
            clip: true

            delegate: Loader {
                property url uri: column_0
                property string iconHint: column_1
                property string groupId: column_2
                property string mimetype: column_3
                property string displayName: column_4
                property string comment: column_5

                width: renderer.cellWidth
                height: renderer.cellHeight

                sourceComponent: cellRenderer
                onLoaded: {
                    item.uri = uri
                    item.iconHint = iconHint
                    item.mimetype = mimetype
                    item.displayName = displayName
                    item.comment = comment
                }
            }

            model: renderer.model
        }
    }
}
