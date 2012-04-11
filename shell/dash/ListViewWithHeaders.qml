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
import Unity2d 1.0

/*
  List item that behaves similarly to a ListView but supports adding headers
  before every delegate.
  It works around the lack of flexibility in section headers positioning of
  ListView (cf. http://bugreports.qt.nokia.com/browse/QTBUG-12880). It also
  supports delegates that are flickable by flicking their content properly
  depending on where you are in the list.

  To use it the following properties need to be set:
  - bodyDelegate: Component used as a template for each item of the model;
  - headerDelegate: Component used as a template for the header preceding each body

  Currently, it only works in a vertical layout.
*/
FocusScope {
    id: list

    property alias flickable: scroll
    property Component bodyDelegate
    property Component headerDelegate
    property alias model: categories.model

    property bool giveFocus: false

    function focusFirstHeader() {
        focusPath.reset()
        focusPath.currentItem.header.forceActiveFocus()
    }

    function focusLastHeader() {
        focusPath.focusLastRow()
        focusPath.currentItem.header.forceActiveFocus()
    }

    function focusNextHeader() {
        var moved = focusPath.moveToNext()
        if (moved) {
            focusPath.currentItem.header.forceActiveFocus()
        }
        return moved
    }

    function focusPreviousHeader() {
        if (!focusPath.currentItem.header.activeFocus) {
            focusPath.currentItem.body.item.focusFirstElement()
            focusPath.currentItem.header.forceActiveFocus()
            return true
        } else {
            var moved = focusPath.moveToPrevious()
            if (moved) {
                focusPath.currentItem.body.item.focusFirstElement()
                focusPath.currentItem.header.forceActiveFocus()
            }
            return moved
        }
    }

    FocusPath {
        id: focusPath
        item: categoriesColumn
        columns: 1
        direction: FocusPath.Vertical
    }

    Flickable {
        id: scroll
        anchors.fill: parent
        clip: true
        interactive: true
        contentWidth: width
        contentHeight: categoriesColumn.height + scrollBottomMargin
        flickableDirection: Flickable.VerticalFlick
        focus: true

        /* Margin used in auto-scroll */
        property int scrollTopMargin: 30
        property int scrollBottomMargin: 30

        Behavior on contentY {
            NumberAnimation { duration:  200; }
        }

        function moveToPosition(item) {
            var itemPosition = item.mapToItem(categoriesColumn, x, y)
            var itemBottom = itemPosition.y + item.height + scrollBottomMargin
            var itemTop = itemPosition.y - scrollTopMargin
            var newContentY = -1;

            if (scroll.contentY > itemTop) {
                newContentY = itemTop
            } else if ((scroll.contentY + scroll.height) < itemBottom) {
                newContentY = itemBottom - scroll.height;
            }

            if (newContentY >= 0) {
                scroll.contentY = newContentY
            }
        }

        Column {
            id: categoriesColumn

            Repeater {
                id: categories
                clip: true
                FocusPath.skip: true

                FocusScope {
                    id: category

                    width: childrenRect.width
                    height: childrenRect.height
                    FocusPath.index: index
                    FocusPath.skip: !headerLoader.item.visible && !bodyLoader.item.visible

                    property alias body: bodyLoader.item
                    property alias header: headerLoader.item

                    Column {

                        Loader {
                            id: headerLoader
                            sourceComponent: headerDelegate
                            width: scroll.width
                            focus: true
                            KeyNavigation.down: bodyLoader

                            /* Workaround Qt bug http://bugreports.qt.nokia.com/browse/QTBUG-18857
                               More documentation at http://bugreports.qt.nokia.com/browse/QTBUG-18011
                             */
                            property int index
                            Binding { target: headerLoader; property: "index"; value: index }
                            property variant model
                            Binding { target: headerLoader; property: "model"; value: model }
                            property variant body
                            Binding { target: headerLoader; property: "body"; value: bodyLoader.item }
                            property int flickerMoving
                            Binding { target: headerLoader; property: "flickerMoving"; value: scroll.moving }

                            onActiveFocusChanged: {
                                if (visible && item && item.activeFocus) {
                                    var categoryOnTopIndex = focusPath.previousIndex()
                                    if (categoryOnTopIndex != -1) {
                                        categories.itemAt(categoryOnTopIndex).body.item.focusLastRow()
                                    }

                                    focusPath.currentItem.body.item.focusFirstElement()
                                    scroll.moveToPosition(item)
                                }
                            }
                            onLoaded: item.focus = true
                        }

                        Loader {
                            id: bodyLoader
                            sourceComponent: list.bodyDelegate
                            width: scroll.width
                            KeyNavigation.up: headerLoader

                            property variant currentItem: item.currentItem ? item.currentItem : null

                            function scrollToActiveItem()
                            {
                                if (visible && currentItem && (activeFocus || currentItem.activeFocus)) {
                                    scroll.moveToPosition(currentItem)
                                }
                            }

                            onCurrentItemChanged: scrollToActiveItem()
                            onActiveFocusChanged: scrollToActiveItem()

                           /* Workaround Qt bug http://bugreports.qt.nokia.com/browse/QTBUG-18857
                              More documentation at http://bugreports.qt.nokia.com/browse/QTBUG-18011
                            */
                            property int index
                            Binding { target: bodyLoader; property: "index"; value: index }
                            property variant model
                            Binding { target: bodyLoader; property: "model"; value: model }
                            onLoaded: item.focus = true
                        }
                    }
                }
            }
        }
    }
}
