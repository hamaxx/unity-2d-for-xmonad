import Qt 4.7

SearchRefineOption {
    id: searchRefineOption

    AbstractButton {
        id: header

        focus: true
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        height: childrenRect.height

        TextCustom {
            id: title

            anchors.top: parent.top
            anchors.left: parent.left

            text: searchRefineOption.title
            font.pixelSize: 16
            font.bold: true
        }
    }

    GridView {
        id: filters

        property int columns: 2
        property int rowsPerColumn: Math.ceil(count/columns)

        cellHeight: 35
        cellWidth: width/columns

        anchors.top: header.bottom
        anchors.topMargin: 15
        height: cellHeight * rowsPerColumn
        boundsBehavior: Flickable.StopAtBounds

        anchors.left: parent.left
        anchors.right: parent.right

        flow: GridView.TopToBottom

        /* Make sure the first item is selected when getting the focus for the first time */
        currentIndex: 0

        delegate: TickBox {
            height: filters.cellHeight
            width: filters.cellWidth-10
            text: column_0
            ticked: dash.currentPage.model.activeSection == model.index

            onClicked: placeEntryModel.activeSection = model.index
        }

        model: placeEntryModel != undefined ? placeEntryModel.sections : undefined
    }
}
