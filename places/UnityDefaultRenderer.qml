import Qt 4.7

RendererGrid {
    cellWidth: 136
    cellHeight: 108
    horizontalSpacing: 10
    verticalSpacing: 10

    cellRenderer: Component {
        Button {
            property url uri
            property string iconHint
            property string mimetype
            property string displayName
            property string comment

            onClicked: {
                dashView.active = false
                placeEntryModel.place.activate(decodeURIComponent(uri.toString()))
            }

            Image {
                id: icon

                source: iconHint != "" ? "image://icons/"+iconHint : ""
                width: 48
                height: 48
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.top: parent.top
                anchors.topMargin: 10
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
                anchors.top: icon.bottom
                anchors.bottom: parent.bottom
                anchors.right: parent.right
                anchors.left: parent.left
                anchors.topMargin: 13
                anchors.bottomMargin: 5
                anchors.rightMargin: 3
                anchors.leftMargin: 3
                font.underline: parent.activeFocus
            }
        }
    }
}
