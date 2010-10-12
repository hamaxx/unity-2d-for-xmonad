import Qt 4.7
import QtDee 1.0
import UnityApplications 1.0

Rectangle {
    width: 800
    height: 600
    color: "grey"

    GridView {
        property int delegate_width: 165
        property int delegate_height: 80
        property int horizontal_spacing: 20
        property int vertical_spacing: 25

        anchors.fill: parent
        cellWidth: delegate_width+horizontal_spacing
        cellHeight: delegate_height+vertical_spacing

        delegate: Application {
            width: GridView.view.delegate_width
            height: GridView.view.delegate_height
            label: column_4
            icon: "image://icons/"+column_1
        }
        model: DeeListModel {
            objectPath: "/com/canonical/dee/model/com/canonical/Unity/ApplicationsPlace/ResultsModel"
            service: "com.canonical.Unity.ApplicationsPlace"
        }
    }
}
