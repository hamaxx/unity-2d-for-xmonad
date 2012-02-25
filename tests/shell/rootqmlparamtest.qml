import QtQuick 1.1

Rectangle {
    height: 400
    width: 400
    color: "white"

    Text {
        text: "This is rootqmlparamtest.qml"
    }
    Component.onCompleted: declarativeView.show()
}
