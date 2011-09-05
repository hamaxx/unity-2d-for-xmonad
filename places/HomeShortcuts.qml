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

Grid {
    anchors.fill: parent
    anchors.topMargin: 21
    anchors.bottomMargin: 35
    anchors.leftMargin: 46
    anchors.rightMargin: 32
    spacing: 51
    columns: 4
    rows: 2

    function selectChild(index) {
        if (index < 0 || index >= children.length) return false
        currentIndex = index
        children[index].focus = true
        return true
    }

    property int currentIndex: 0
    Keys.onPressed: if (handleKeyPress(event.key)) event.accepted = true
    function handleKeyPress(key) {
        switch (key) {
        case Qt.Key_Right:
            return selectChild(currentIndex+1)
        case Qt.Key_Left:
            return selectChild(currentIndex-1)
        case Qt.Key_Up:
            return selectChild(currentIndex-columns)
        case Qt.Key_Down:
            return selectChild(currentIndex+columns)
        }
    }

    HomeButton {
        focus: true
        label: u2d.tr("Media Apps")
        icon: "artwork/find_media_apps.png"
        onClicked: activateLensWithOptionFilter("applications.lens", "type", "media")
    }

    HomeButton {
        label: u2d.tr("Internet Apps")
        icon: "artwork/find_internet_apps.png"
        onClicked: activateLensWithOptionFilter("applications.lens", "type", "internet")
    }

    HomeButton {
        label: u2d.tr("More Apps")
        icon: "artwork/find_more_apps.png"
        onClicked: activateLensAndClearFilter("applications.lens", "type")
    }

    HomeButton {
        label: u2d.tr("Find Files")
        icon: "artwork/find_files.png"
        onClicked: activateLens("files.lens")
    }

    /* FIXME: use user's preferred applications instead of hardcoding them */
    HomeButtonDefaultApplication {
        label: u2d.tr("Browse the Web")
        contentType: "x-scheme-handler/http"
    }

    HomeButtonDefaultApplication {
        label: u2d.tr("View Photos")
        contentType: "image/jpeg"
        desktopFile: "shotwell-viewer.desktop"
    }

    HomeButtonDefaultApplication {
        label: u2d.tr("Check Email")
        contentType: "x-scheme-handler/mailto"
    }

    HomeButtonDefaultApplication {
        label: u2d.tr("Listen to Music")
        contentType: "audio/x-vorbis+ogg"
    }
}

