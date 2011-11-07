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

FocusScope {
    FocusPath.skip: focusSkip
    FocusPath.index: focusIndex

    property bool enabled: true
    property int focusIndex: 0
    property bool focusSkip: !visible



    /* Use to manually set the "pressed" state of the button. This is not
       necessary in the normal use case, but is useful when a child item eats
       the mouse events (e.g. a DragArea).
       This is a clumsy workaround for the lack of a MouseProxy element
       (see http://bugreports.qt.nokia.com/browse/QTBUG-13007). */
    property bool pressed: false

    signal clicked

    Accessible.role: Accessible.PushButton

    MouseArea {
        id: mouse_area

        /* FIXME: workaround double click bug
                  http://bugreports.qt.nokia.com/browse/QTBUG-12250 */
        property bool double_clicked: false

        enabled: parent.enabled
        hoverEnabled: parent.enabled
        anchors.fill: parent
        onClicked: {
            if(double_clicked)
                double_clicked = false
            else
                parent.clicked()
        }
        onDoubleClicked: {
            double_clicked = true
        }
    }

    states: [
        State {
            name: "pressed"
            when: pressed || mouse_area.pressed
        },
        State {
            name: "hovered"
            when: mouse_area.containsMouse
        },
        State {
            name: "selected"
            when: activeFocus
        }
    ]

    Keys.onPressed: {
        if (event.key == Qt.Key_Return) {
            clicked()
            event.accepted = true;
        }
    }
}
