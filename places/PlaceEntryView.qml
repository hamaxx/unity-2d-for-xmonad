import Qt 4.7
import Unity2d 1.0 /* Necessary for QSortFilterProxyModelQML */

Item {
    id: placeEntryView

    /* An instance of PlaceEntryModel */
    property variant model

    ListViewWithScrollbar {
        id: results

        anchors.fill: parent

        /* The group's delegate is chosen dynamically depending on what
           groupRenderer is returned by the GroupsModel.

           Each groupRenderer should have a corresponding QML file with the
           same name that will be used as delegate.
           For example:

           If groupRenderer == 'UnityShowcaseRenderer' then it will look for
           the file 'UnityShowcaseRenderer.qml' and use it to render the group.
        */
        list.delegate: Loader {
            property string groupRenderer: column_0
            property string displayName: column_1
            property string iconHint: column_2
            property int groupId: index

            source: groupRenderer ? groupRenderer+".qml" : ""
            onStatusChanged: {
                if (status == Loader.Error)
                    console.log("Failed to load renderer", groupRenderer)
            }

            /* -2 is here to prevent clipping of the group; it looks like a bug */
            width: ListView.view.width-2

            /* Model that will be used by the group's delegate */
            QSortFilterProxyModelQML {
                id: group_model

                model: placeEntryView.model.entryResultsModel

                /* resultsModel contains data for all the groups of a given Place.
                   Each row has a column (the second one) containing the id of
                   the group it belongs to (groupId).
                */
                filterRole: 2 /* second column (see above comment) */
                filterRegExp: RegExp("^%1$".arg(groupId)) /* exact match */
            }

            onLoaded: {
                item.parentListView = results.list
                item.displayName = displayName
                item.iconHint = iconHint
                item.groupId = groupId
                item.model = group_model
                item.placeEntryModel = placeEntryView.model
            }
        }

        list.model: placeEntryView.model != undefined ? placeEntryView.model.entryGroupsModel : undefined
    }
}
