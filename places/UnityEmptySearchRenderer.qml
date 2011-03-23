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

/* Used to convey to the user that the current section is empty.
   The first row of the results model contains the message to the user in the
   display_name column.
   Any subsequent rows contain optional alternative actions the user may activate.
   These actions are URI encoded, so fx. a Try searching for 'pony' on Google
   might be encoded with http://www.google.com/#q=pony in the uri column of the
   second row and the string "Try searching for <i>pony</i> on Google" in the
   display_name column.
*/
Renderer {
    id: renderer

    property int resultHeight: 45
    height: results.count*resultHeight

    ListView {
        id: results

        anchors.fill: parent
        boundsBehavior: ListView.StopAtBounds
        orientation: ListView.Vertical

        model: renderer.model
        delegate: Button {
            property string uri: column_0
            property string iconHint: column_1
            property string groupId: column_2
            property string mimetype: column_3
            property string displayName: column_4
            property string comment: column_5

            width: 336
            height: resultHeight
            /* If no uri is available then disable the button entirely (no
               hovering, no clicking, etc. */
            enabled: uri != ""
            onClicked: {
                dashView.active = false
                placeEntryModel.place.activate(uri)
            }

            TextCustom {
                text: displayName
                font.pixelSize: 16
                anchors.left: parent.left
                anchors.leftMargin: 8
                anchors.right: parent.right
                height: paintedHeight
                anchors.verticalCenter: parent.verticalCenter
            }
        }
    }
}
