/*
 * This file is part of unity-2d
 *
 * Copyright 2010-2011 Canonical Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 3.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

import Qt 4.7
import UnityApplications 1.0 /* Necessary for the ImageProvider serving image://icons */

/* Renderers typically use a grid layout to render the model. The RendererGrid
   component provides a standard implementation of such a layout where the
   cells can be customized by providing a QML component to it.
   A user of RendererGrid would create a renderer inheriting from it
   and pass a Component via the 'cellRenderer' property.
*/
Renderer {
    id: renderer

    property variant cellRenderer
    property bool folded
    folded: {
        /* Look for the groupId as a complete word inside the list of expanded groups.
           Examples of ExpandedGroups hint: "2", "1 3 7", "1 2", etc.
         */
        var re = RegExp("\\b%1\\b".arg(renderer.groupId))
        var expandedGroups = placeEntryModel.entryRendererHints["ExpandedGroups"]
        return !re.test(expandedGroups)
    }

    property int cellWidth: 158
    property int cellHeight: 76
    property int horizontalSpacing: 26
    property int verticalSpacing: 26

    /* Using results.contentHeight produces binding loop warnings and potential
       rendering issues. We compute the height manually.
    */
    /* FIXME: tricking the system by making the delegate of height 0 and with
              an invisible header is no good: the item in the model still
              exists and some things such as keyboard selection break.
    */
    height: results.count > 0 ? header.height + results_layout.anchors.topMargin + results.totalHeight : 0
    //Behavior on height {NumberAnimation {duration: 200}}

    GroupHeader {
        id: header

        visible: results.count > 0
        availableCount: results.count - results.cellsPerRow
        folded: parent.folded
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        height: 32
        icon: parent.iconHint
        label: parent.displayName

        onClicked: parent.folded = !parent.folded
    }

    Item {
        id: results_layout

        anchors.top: header.bottom
        anchors.topMargin: 22
        anchors.left: parent.left
        anchors.leftMargin: 2
        anchors.right: parent.right
        anchors.bottom: parent.bottom

        CenteredGridView {
            id: results

            /* FIXME: this is a gross hack compensating for the lack of sections
               in GridView (see ListView.section).

               We nest GridViews inside a ListView and add headers manually
               (GroupHeader). The total height of each Group is computed
               manually and given back to the ListView. However that size cannot
               be used by the individual GridViews because it would make them
               load all of their delegates at once using far too much memory and
               processing power. Instead we constrain the height of the GridViews
               and compute their position manually to compensate for the position
               changes when flicking the ListView.

               We assume that renderer.parentListView is the ListView we nest our
               GridView into.
            */
            property variant flickable: renderer.parentListView

            /* flickable.contentY*0 is equal to 0 but is necessary in order to
               have the entire expression being evaluated at the right moment.
            */
            property int inFlickableY: flickable.contentY*0+parent.mapToItem(flickable, 0, 0).y
            /* note: testing for flickable.height < 0 is probably useless since it is
               unlikely flickable.height will ever be negative.
            */
            property int compensateY: inFlickableY > 0 || flickable.height < 0 || totalHeight < flickable.height ? 0 : -inFlickableY

            /* Synchronise the position and content's position of the GridView
               with the current position of flickable's visibleArea */
            function synchronisePosition() {
                y = compensateY
                contentY = compensateY
            }

            onCompensateYChanged: synchronisePosition()
            /* Any change in content needs to trigger a synchronisation */
            onCountChanged: synchronisePosition()
            onModelChanged: synchronisePosition()

            width: flickable.width
            height: Math.min(totalHeight, flickable.height)

            /* Only display one line of items when folded */
            property int displayedCount: folded ? cellsPerRow : count
            property int totalHeight: results.cellHeight*Math.ceil(displayedCount/cellsPerRow)

            minHorizontalSpacing: renderer.horizontalSpacing
            minVerticalSpacing: renderer.verticalSpacing
            delegateWidth: renderer.cellWidth
            delegateHeight: renderer.cellHeight

            interactive: false
            clip: true

            delegate: Item {

                width: results.cellWidth
                height: results.cellHeight

                Loader {
                    property string uri: column_0
                    property string iconHint: column_1
                    property string groupId: column_2
                    property string mimetype: column_3
                    property string displayName: column_4
                    property string comment: column_5

                    width: results.delegateWidth
                    height: results.delegateHeight
                    anchors.horizontalCenter: parent.horizontalCenter

                    sourceComponent: cellRenderer
                    onLoaded: {
                        item.uri = uri
                        item.iconHint = iconHint
                        item.mimetype = mimetype
                        item.displayName = displayName
                        item.comment = comment
                    }
                }
            }

            model: renderer.model
        }
    }
}
