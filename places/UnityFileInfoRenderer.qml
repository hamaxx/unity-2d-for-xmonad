import Qt 4.7

RendererGrid {
    cellWidth: 208
    cellHeight: 57
    horizontalSpacing: 26
    verticalSpacing: 26

    cellRenderer: Component {
        Button {
            id: button

            property url uri
            property string iconHint
            property string mimetype
            property string displayName
            property string comment
            property string parentDirectory
            parentDirectory: {
                var slashIndex, path, directoryName
                var path = uri.toString()

                /* Remove the trailing file name */
                slashIndex = path.lastIndexOf("/")
                path = path.substring(0, slashIndex)
                /* Remove the full path before the directory name */
                slashIndex = path.lastIndexOf("/")
                directoryName = path.substring(slashIndex+1)

                return directoryName
            }

            onClicked: placeEntryModel.place.activate(uri.toString())

            Image {
                id: icon

                source: "image://icons/"+iconHint
                width: 48
                height: 48
                anchors.verticalCenter: parent.verticalCenter
                anchors.left: parent.left
                anchors.leftMargin: 3
                fillMode: Image.PreserveAspectFit
                sourceSize.width: width
                sourceSize.height: height

                asynchronous: true
                opacity: status == Image.Ready ? 1 : 0
                Behavior on opacity {NumberAnimation {duration: 200; easing.type: Easing.InOutQuad}}
            }

            Column {
                id: labels

                anchors.top: parent.top
                anchors.topMargin: 3
                anchors.bottom: parent.bottom
                anchors.bottomMargin: 3
                anchors.left: icon.right
                anchors.leftMargin: 15
                anchors.right: parent.right
                anchors.rightMargin: 3

                TextCustom {
                    text: displayName
                    color: button.state == "pressed" ? "#5e5e5e" : "#ffffff"
                    elide: Text.ElideMiddle

                    width: parent.width
                    height: paintedHeight
                }

                TextCustom {
                    text: parentDirectory
                    color: button.state == "pressed" ? "#7e7e7e" : "#c8c8c8"
                    elide: Text.ElideMiddle

                    width: parent.width
                    height: paintedHeight
                }

                TextCustom {
                    text: comment
                    color: button.state == "pressed" ? "#5e5e5e" : "#ffffff"
                    elide: Text.ElideMiddle

                    width: parent.width
                    height: paintedHeight
                }
            }
        }
    }
}
