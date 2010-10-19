import Qt 4.7
import QtDee 1.0
import UnityApplications 1.0 /* Necessary for the ImageProvider serving image://icons */
import UnityPlaces 1.0
import "utils.js" as Utils

Item {
    property int groupNumber
    property alias icon: header.icon
    property alias label: header.label
    property bool folded: true
    property variant placeResultsModel
    property variant placeDBusInterface

    /* Using results.contentHeight produces binding loop warnings and potential
       rendering issues. We compute the height manually.
    */
    /* FIXME: tricking the system by making the delegate of height 0 and with
              an invisible header is no good: the item in the model still
              exists and some things such as keyboard selection break.
    */
    height: results.count > 0 ? header.height + results.anchors.topMargin + results.totalHeight : 0
    //Behavior on height {NumberAnimation {duration: 200}}

    GroupHeader {
        id: header

        visible: results.count > 0
        moreAvailable: results.count >= results.cellsPerLine
        folded: parent.folded
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        height: 28

        onClicked: parent.folded = !parent.folded
    }

    Item {
        anchors.top: header.bottom
        anchors.topMargin: 14
        anchors.left: parent.left
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
            */
            property variant flickable: place_results

            /* flickable.contentY*0 is equal to 0 but is necessary in order to
               have the entire expression being evaluated at the right moment.
            */
            property int inFlickableY: flickable.contentY*0+parent.mapToItem(flickable, 0, 0).y
            property int compensateY: inFlickableY > 0 ? 0 : -inFlickableY

            width: flickable.width
            height: flickable.height
            y: compensateY
            contentY: compensateY


            property int delegate_width: 158
            property int delegate_height: 76
            property int horizontal_spacing: 20
            property int vertical_spacing: 25
            property int cellsPerLine: Math.floor(width/cellWidth)
            property int totalHeight: cellHeight*Math.ceil(count/cellsPerLine)


            cellWidth: delegate_width+horizontal_spacing
            cellHeight: delegate_height+vertical_spacing

            interactive: false
            clip: true

            delegate: Result {
                width: GridView.view.delegate_width
                height: GridView.view.delegate_height
                label: column_4
                icon: "image://icons/"+column_1

                onClicked: {
                    var uri = column_0
                    if(!placeDBusInterface.Activate(uri)) {
                        var matches = uri.match("^(.*)(?:://)(.*)$")
                        var schema = matches[1]
                        var path = matches[2]
                        if(schema == "application") {
                            Utils.launchApplicationFromDesktopFile(path, parent)
                        }
                        else {
                            console.log("FIXME: Possibly no handler for schema \'%1\'".arg(schema))
                            console.log("Trying to open", uri)
                            /* Try our luck */
                            /* FIXME: uri seems already escaped though
                                      Qt.openUrlExternally tries to escape it */
                            Qt.openUrlExternally(uri)
                        }
                    }
                }
            }

            /* placeResultsModel contains data for all the Groups of a given Place.
               Each row has a column (the seconds one) containing its Group number.
            */
            model: QSortFilterProxyModelQML {
                filterRole: 2
                filterRegExp: RegExp(groupNumber)
                limit: folded ? results.cellsPerLine : -1
                model: placeResultsModel
            }
        }
    }
}
