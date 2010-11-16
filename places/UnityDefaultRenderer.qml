import Qt 4.7

RendererGrid {
    cellRenderer: Component {
        Button {
            property alias icon: icon.source
            property alias label: label.text
            property string uri

            onClicked: place.activate(uri)

            Image {
                id: icon

                width: 48
                height: 48
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.top: parent.top
                anchors.topMargin: 3
                fillMode: Image.PreserveAspectFit
                sourceSize.width: width
                sourceSize.height: height

                asynchronous: true
                opacity: status == Image.Ready ? 1 : 0
                Behavior on opacity {NumberAnimation {duration: 200; easing.type: Easing.InOutQuad}}
            }

            TextCustom {
                id: label

                color: parent.state == "pressed" ? "#444444" : "#ffffff"
                elide: Text.ElideMiddle
                horizontalAlignment: Text.AlignHCenter
                anchors.bottom: parent.bottom
                anchors.right: parent.right
                anchors.left: parent.left
                anchors.bottomMargin: 5
                anchors.rightMargin: 3
                anchors.leftMargin: 3
                font.underline: parent.activeFocus
            }
        }
    }
}
