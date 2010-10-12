import Qt 4.7

ListView {
    width: 200
    height: 200
    delegate: Text {
        x: 66
        y: 93
        text: column_0
    }
    model: DeeListModel {
        objectPath: "/com/canonical/dee/model/com/canonical/Unity/ApplicationsPlace/SectionsModel"
        service: "com.canonical.Unity.ApplicationsPlace"
    }
}
