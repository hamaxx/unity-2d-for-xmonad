import QtQuick 1.1
import Unity2d 1.0

Rectangle {
    color: "lightsteelblue"
    width: 300
    height: 300
    ListModel {
        id : myModel
        ListElement { name: "Item0" }
        ListElement { name: "Item1" }
        ListElement { name: "Item2" }
        ListElement { name: "Item3" }
        ListElement { name: "Item4" }
        ListElement { name: "Item5" }
        ListElement { name: "Item6" }
        ListElement { name: "Item7" }
        ListElement { name: "Item8" }
    }

    FocusPath {
        item: grid
        objectName: "focusPath"
        columns: grid.columns
        flow: grid.flow
    }

    Grid {
        id: grid
        objectName: "gridLayout"
        columns: 3

        function insertItem(index, itemName) {
            myModel.insert(index, {"name" : itemName})
        }

        Repeater {
            model: myModel
            FocusPath.skip: true
            FocusScope {
                width: 50
                height: 50
                property string itemData: name
                FocusPath.index: index
                Rectangle {
                    color: activeFocus ? "green" : "purple"
                    anchors.fill: parent
                    focus: true
                }
            }
        }
    }
}
