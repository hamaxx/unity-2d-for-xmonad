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

import QtQuick 1.1
import Unity2d 1.0

/*
  List item that behaves similarly to a ListView but supports adding headers
  before every delegate.
  It works around the lack of flexibility in section headers positioning of
  ListView (cf. http://bugreports.qt.nokia.com/browse/QTBUG-12880). It also
  supports delegates that are flickable by flicking their content properly
  depending on where you are in the list.

  To use it the following properties need to be set:
  - bodyDelegate: Component used as a template for each item of the model; it
  - headerDelegate: Component used as a template for the header preceding each body

  Currently, it only works in a vertical layout.
*/
FocusScope {
    id: list
    anchors.fill: parent
    focus: true

    property alias flickable: scroll
    property Component bodyDelegate
    property Component headerDelegate
    property alias model: content.model

    property bool giveFocus: false

    FocusPath {
        id: focusPath
        item: root
        columns: 1
    }

    onVisibleChanged: {
        focusPath.reset();
    }

    Flickable {
        id: scroll
        anchors.fill: parent
        clip: true
        interactive: true
        contentWidth: width
        contentHeight: root.height
        flickableDirection: Flickable.VerticalFlick
        focus: true

        Behavior on contentY {
            NumberAnimation { duration:  300; }
        }

        function moveToPosition(itemPos, itemHeight) {
            var itemBottom = itemPos.y + itemHeight
            var itemTop = itemHeight * 0.30
            if (itemPos.y < itemTop) {
                itemTop = itemPos.y
            } else {
                itemTop = itemPos.y - 30
            }

            var scrollPos = -1;

            if (scroll.contentY > itemTop) {
                scrollPos = itemTop
            } else if ((scroll.contentY + scroll.height) < itemBottom) {
                scrollPos = itemBottom - scroll.height;
            }

            if (scrollPos >= 0) {
                scroll.contentY = scrollPos
            }
        }

        Column {
            id: root

            Repeater {
                id: content
                clip: true
                focus: true
                FocusPath.skip: true

                FocusScope {
                    focus: false
                    width: childrenRect.width
                    height: rowItem.height
                    visible: bodyLoader.height > 0
                    FocusPath.index: index
                    FocusPath.skip: !bodyLoader.item || !bodyLoader.visible || !headerLoader.item

                    Column {
                        id: rowItem
                        focus: true

                        Loader {
                            id: headerLoader
                            sourceComponent: headerDelegate
                            width: scroll.width
                            visible: item && bodyLoader.visible
                            focus: item && item.visible
                            KeyNavigation.down: bodyLoader

                            property int index
                            Binding { target: headerLoader; property: "index"; value: index }
                            property variant model
                            Binding { target: headerLoader; property: "model"; value: model }
                            property variant body
                            Binding { target: headerLoader; property: "body"; value: bodyLoader.item }

                            onActiveFocusChanged: {
                                if (visible && item && item.activeFocus) {
                                    var itemPos = item.mapToItem(root, x, y)
                                    scroll.moveToPosition(itemPos, item.height)
                                }
                            }
                        }

                        Loader {
                            id: bodyLoader
                            sourceComponent: list.bodyDelegate
                            width: scroll.width
                            focus: item.visible
                            KeyNavigation.up: headerLoader

                            property variant currentItem: item.currentItem ? item.currentItem : null

                            function scrollToActiveItem()
                            {
                                if (visible && currentItem && (activeFocus || currentItem.activeFocus)) {
                                    var itemPos = currentItem.mapToItem(root, x, y)
                                    scroll.moveToPosition(itemPos, currentItem.height)
                                }
                            }

                            onCurrentItemChanged: scrollToActiveItem()
                            onActiveFocusChanged: scrollToActiveItem()

                            property int index
                            Binding { target: bodyLoader; property: "index"; value: index }
                            property variant model
                            Binding { target: bodyLoader; property: "model"; value: model }
                        }
                    }
                }
            }
        }
    }
}
