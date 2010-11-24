import Qt 4.7

RendererGrid {
    cellWidth: 158
    cellHeight: 76
    horizontalSpacing: 26
    verticalSpacing: 26

    cellRenderer: Component {
        Button {
            property url uri
            property string iconHint
            property string mimetype
            property string displayName
            property string comment

            onClicked: place.activate(uri.toString())

            Image {
                id: icon

                source: "image://icons/"+iconHint
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

                text: displayName
                color: parent.state == "pressed" ? "#5e5e5e" : "#ffffff"
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
