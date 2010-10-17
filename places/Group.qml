import Qt 4.7
import QtDee 1.0
import UnityApplications 1.0 /* Necessary for the ImageProvider serving image://icons */
import UnityPlaces 1.0

Item {
    property int groupNumber
    property alias icon: header.icon
    property alias label: header.label
    property bool folded: true
    property variant placeResultsModel
    property variant placeDBusInterface

    /* Using group.contentHeight produces binding loop warnings and potential
       rendering issues. We compute the height manually.
    */
    height: group.count > 0 ? header.height + group.anchors.topMargin + group.totalHeight : 0
    visible: group.count > 0

    GroupHeader {
        id: header

        folded: parent.folded
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        height: 27

        onClicked: parent.folded = !parent.folded
    }

    GridView {
        id: group

        anchors.top: header.bottom
        anchors.topMargin: 14
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom

        property int delegate_width: 165
        property int delegate_height: 80
        property int horizontal_spacing: 20
        property int vertical_spacing: 25
        property int cellsPerLine: width/cellWidth
        property int totalHeight: count <= cellsPerLine ? cellHeight : cellHeight*count/cellsPerLine


        cellWidth: delegate_width+horizontal_spacing
        cellHeight: delegate_height+vertical_spacing

        interactive: false

        delegate: Result {
            width: GridView.view.delegate_width
            height: GridView.view.delegate_height
            label: column_4
            icon: "image://icons/"+column_1

            onClicked: {
                var uri = column_0
                if(!placeDBusInterface.Activate(uri))
                {
                    console.log("FIXME: Possibly no handler for", uri)
                    /* Try our luck */
                    Qt.openUrlExternally(uri)
                }
            }
        }

        /* placeResultsModel contains data for all the Groups of a given Place.
           Each row has a column (the seconds one) containing its Group number.
        */
        model: QSortFilterProxyModelQML {
            filterRole: 2
            filterRegExp: RegExp(groupNumber)
            limit: folded ? group.cellsPerLine : -1
            model: placeResultsModel
        }
    }
}
