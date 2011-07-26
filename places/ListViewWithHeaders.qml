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

/*
  Ref.: http://bugreports.qt.nokia.com/browse/QTBUG-12880
*/
FocusScope {
    id: list

    property alias flickable: mouse
    property alias model: repeater.model
    property bool accordion: false
    /* bodyDelegate must be an item that has the following properties:
        - 'contentY'
        - 'totalHeight'
        - 'currentItem'
    */
    property Component bodyDelegate
    property Component headerDelegate
    property int currentIndex: -1
    /* FIXME: Should be read-only but it is not possible to define a read-only property from QML.
       Ref.: https://bugreports.qt.nokia.com//browse/QTBUG-15257
    */
    property variant currentItem: items.childFromIndex(currentIndex)


    clip: true

    /* updateMouseContentY() needs to be called when any of the variable
       involved in the computation of the 'y' property of the currentSubItem changes.

       if accordion is false:
       - heightFirstChildren(index)
       - headerLoader.height
       - list.height

       if accordion is true:
       - heightFirstChildren(index)
       - items.availableHeight
       - heightFirstHeaders(index)

       items.contentHeight seems to depend on all of those so it's enough to
       depend only on it.
    */
    function updateMouseContentY() {
        if (currentSubItem != undefined) {
            mouse.contentY = Math.max(currentSubItem.mapToItem(mouse.contentItem, 0, 0).y -list.height/2, 0)
        }
    }
    property variant currentSubItem: currentItem != undefined ? currentItem.bodyLoader.item.currentItem : undefined
    onCurrentSubItemChanged: updateMouseContentY()

    Connections {
        target: items
        onContentHeightChanged: updateMouseContentY()
    }

    FocusScope {
        id: items

        property int availableHeight: list.height - heightFirstHeaders(repeater.count)
        property int contentHeight: items.heightFirstChildren(repeater.count)
        property real value: mouse.contentY

        anchors.fill: parent

        function heightFirstChildren(n) {
            var i
            var totalHeight = 0
            /* items.children contains both the repeated items and the repeater
               itself. Skip and ignore the repeater. */
            for (i=0; i<n && i<children.length; i++) {
                if(children[i] == repeater) {n += 1; continue}
                totalHeight += children[i].height
            }
            return totalHeight
        }

        function heightFirstHeaders(n) {
            var i
            var totalHeight = 0
            /* items.children contains both the repeated items and the repeater
               itself. Skip and ignore the repeater. */
            for (i=0; i<n && i<children.length; i++) {
                if(children[i] == repeater) {n += 1; continue}
                totalHeight += children[i].headerLoader.height
            }
            return totalHeight
        }

        function clamp(x, min, max) {
            return Math.max(Math.min(x, max), min)
        }


        /* Keyboard navigation */
        function isIndexValid(index) {
            /* Assuming that children contains exactly one item that is not a child (repeater) */
            return index >= 0 && index < children.length-1
        }

        focus: true
        Keys.onPressed: if (handleKeyPress(event.key)) event.accepted = true
        function handleKeyPress(key) {
            switch (key) {
            case Qt.Key_Down:
                return selectNextEnabled()
            case Qt.Key_Up:
                return selectPreviousEnabled()
            }
        }

        function childFromIndex(index) {
            var indexInChildren = 0
            for(var i=0; i<children.length; i++) {
                if (children[i] != repeater) {
                    if (indexInChildren == index) return children[i]
                    indexInChildren++
                }
            }

            return undefined
        }

        function selectNextEnabled() {
            var index = currentIndex
            do {
                index += 1
                if (!isIndexValid(index)) return false
            } while(!childFromIndex(index).focusable)
            currentIndex = index
            return true
        }

        function selectPreviousEnabled() {
            var index = currentIndex
            do {
                index -= 1
                if (!isIndexValid(index)) return false
            } while(!childFromIndex(index).focusable)
            currentIndex = index
            return true
        }

        property bool needsFocus: false
        onChildrenChanged: {
            /* FIXME: this workarounds the fact that list.focus is set to false
                      when the child with focus is destroyed
            */
            if (needsFocus) {
                list.focus = true
                needsFocus = false
            }
            /* Assuming that children contains exactly one item that is not a child (repeater) */
            if(children.length <= 1) {
                list.currentIndex = -1
            } else {
                list.currentIndex = -1
                selectNextEnabled()
            }
        }

        Repeater {
            id: repeater

            FocusScope {
                property alias bodyLoader: bodyLoader
                property alias headerLoader: headerLoader

                focus: index == list.currentIndex
                Component.onDestruction: items.needsFocus = true

                width: list.width
                height: headerLoader.height + bodyLoader.item.totalHeight
                property bool focusable: bodyLoader.item.focusable

                property int pmin: pmax - (ymax - ymin)
                property int pmax: items.heightFirstChildren(index) - ymin
                property int ymin: list.accordion ? items.heightFirstHeaders(index) : -headerLoader.height
                property int ymax: list.accordion ? ymin + items.availableHeight : list.height
                y: items.clamp(-items.value + ymax + pmin, ymin, ymax)

                Loader {
                    id: headerLoader

                    focus: visible
                    KeyNavigation.down: bodyLoader
                    sourceComponent: headerDelegate
                    onLoaded: item.focus = true
                    width: parent.width

                    /* Workaround Qt bug http://bugreports.qt.nokia.com/browse/QTBUG-18857
                       More documentation at http://bugreports.qt.nokia.com/browse/QTBUG-18011
                    */
                    property int index
                    Binding { target: headerLoader; property: "index"; value: index }
                    property variant model
                    Binding { target: headerLoader; property: "model"; value: model }
                    property variant body
                    Binding { target: headerLoader; property: "body"; value: bodyLoader.item }
                }

                Loader {
                    id: bodyLoader

                    focus: !headerLoader.focus
                    KeyNavigation.up: headerLoader
                    sourceComponent: list.bodyDelegate
                    onLoaded: item.focus = true
                    width: parent.width
                    anchors.top: headerLoader.bottom
                    height: items.clamp(parent.ymax - parent.y, 0, item.totalHeight)

                    Binding {
                        target: bodyLoader.item
                        property: "contentY"
                        value: Math.max(items.value - pmax, 0)
                    }

                    /* Workaround Qt bug http://bugreports.qt.nokia.com/browse/QTBUG-18857
                       More documentation at http://bugreports.qt.nokia.com/browse/QTBUG-18011
                    */
                    property int index
                    Binding { target: bodyLoader; property: "index"; value: index }
                    property variant model
                    Binding { target: bodyLoader; property: "model"; value: model }
                }
            }
        }
    }

    Flickable {
        id: mouse

        z: -1
        anchors.fill: parent
        contentWidth: parent.width
        contentHeight: items.contentHeight
    }
}
