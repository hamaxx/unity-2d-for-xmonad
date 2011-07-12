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

import QtQuick 1.0

Item {
    property alias list: list
    property alias scrollbar: scrollbar

    ListView {
        id: list

        anchors.top: parent.top
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        anchors.right: scrollbar.left
        anchors.rightMargin: 15

        clip: true
        /* FIXME: proper spacing cannot be set because of the hack in Group.qml
           whereby empty groups are still in the list but invisible and of
           height 0.
        */
        //spacing: 31

        orientation: ListView.Vertical

        /* WARNING - HACK - FIXME
           Issue:
           User wise annoying jumps in the list are observable if cacheBuffer is
           set to 0 (which is the default value). States such as 'folded' are
           lost when scrolling a lot.

           Explanation:
           The height of the Group delegate depends on its content. However its
           content is not known until the delegate is instantiated because it
           depends on the number of results displayed by its GridView.

           Resolution:
           We set the cacheBuffer to the biggest possible int in order to make
           sure all delegates are always instantiated.
        */
        cacheBuffer: 2147483647
    }

    Scrollbar {
        id: scrollbar

        anchors.top: parent.top
        anchors.topMargin: 15
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 10
        anchors.right: parent.right

        targetFlickable: list

        /* Hide the scrollbar if there is less than a page of results */
        opacity: targetFlickable.visibleArea.heightRatio < 1.0 ? 1.0 : 0.0
        Behavior on opacity {NumberAnimation {duration: 100}}
    }
}
