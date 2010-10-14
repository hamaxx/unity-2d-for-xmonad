import Qt 4.7
import QtDee 1.0
import UnityApplications 1.0
import UnityPlaces 1.0

Rectangle {
    width: 800
    height: 600
    color: "grey"

    UnityPlaceEntry {
        id: applications_place

        service: "com.canonical.Unity.ApplicationsPlace"
        objectPath: "/com/canonical/unity/applicationsplace/applications"
    }

    BamfMatcher {
        id: bamf_matcher

        service: "org.ayatana.bamf"
        objectPath: "/org/ayatana/bamf/matcher"
    }

    ListView {
        id: sections

        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        height: 20
        orientation: ListView.Horizontal

        delegate: Section {
            width: 100
            height: ListView.view.height
            label: column_0

            onClicked: {
                ListView.view.currentIndex = model.index
                applications_place.SetActiveSection(model.index)
            }
        }

        model: DeeListModel {
            service: "com.canonical.Unity.ApplicationsPlace"
            objectPath: "/com/canonical/dee/model/com/canonical/Unity/ApplicationsPlace/SectionsModel"
        }
    }

    GridView {
        id: results

        property int delegate_width: 165
        property int delegate_height: 80
        property int horizontal_spacing: 20
        property int vertical_spacing: 25

        anchors.top: sections.bottom
        anchors.topMargin: 20
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        cellWidth: delegate_width+horizontal_spacing
        cellHeight: delegate_height+vertical_spacing
        clip: true

        delegate: Result {
            /*property variant view: BamfView {
                service: "org.ayatana.bamf"
                objectPath: modelData
            }*/
            width: GridView.view.delegate_width
            height: GridView.view.delegate_height
            label: column_4
            icon: "image://icons/"+column_1
        }

        model: DeeListModel {
            service: "com.canonical.Unity.ApplicationsPlace"
            objectPath: "/com/canonical/dee/model/com/canonical/Unity/ApplicationsPlace/ResultsModel"
        }
    }
}
