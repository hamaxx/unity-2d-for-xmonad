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
import Unity2d 1.0 /* Necessary for the ImageProvider serving image://icons/theme_name/icon_name */

Flow {
    anchors.fill: parent
    anchors.topMargin: 26
    anchors.bottomMargin: 35
    anchors.leftMargin: 32
    anchors.rightMargin: 32
    spacing: 61

    /* FIXME: dummy icons need to be replaced by design's */
    HomeButton {
        label: u2d.tr("Media Apps")
        icon: "artwork/find_media_apps.png"
        onClicked: activatePlaceEntry("/usr/share/unity/places/applications.place", "Files", 9)
    }

    HomeButton {
        label: u2d.tr("Internet Apps")
        icon: "artwork/find_internet_apps.png"
        onClicked: activatePlaceEntry("/usr/share/unity/places/applications.place", "Files", 8)
    }

    HomeButton {
        label: u2d.tr("More Apps")
        icon: "artwork/find_more_apps.png"
        onClicked: activatePlaceEntry("/usr/share/unity/places/applications.place", "Files", 0)
    }

    HomeButton {
        label: u2d.tr("Find Files")
        icon: "artwork/find_files.png"
        onClicked: activatePlaceEntry("/usr/share/unity/places/files.place", "Files", 0)
    }

    /* FIXME: use user's preferred applications instead of hardcoding them */
    HomeButtonApplication {
        label: u2d.tr("Browse the Web")
        key: "/desktop/gnome/applications/browser/exec"
    }

    HomeButtonApplication {
        label: u2d.tr("View Photos")
        desktopFile: "shotwell.desktop"
    }

    HomeButtonApplication {
        label: u2d.tr("Check Email")
        key: "/desktop/gnome/url-handlers/mailto/command"
    }

    HomeButtonApplication {
        label: u2d.tr("Listen to Music")
        key: "/desktop/gnome/applications/media/exec"
    }
}

