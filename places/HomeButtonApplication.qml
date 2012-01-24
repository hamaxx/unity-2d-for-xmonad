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
import gconf 1.0

HomeButton {
    property alias key: gconfKey.key
    property string desktopFile

    GConfItem {
        id: gconfKey
    }

    LauncherApplication {
        id: application
        desktop_file: {
            var exec = gconfKey.value
            if (!exec) {
                /* Invalid key or value, fall back on the desktop file passed
                   by the client. */
                return desktopFile
            }
            /* Ignore extra parameters. */
            exec = exec.split(" ", 1)[0]
            /* The value may contain either the full path to the executable
               (e.g. "/usr/bin/chromium-browser") or only its basename
               (e.g. "rhythmbox"). */
            var lastSlashPos = exec.lastIndexOf("/")
            if (lastSlashPos != -1) {
                exec = exec.substring(lastSlashPos + 1)
            }
            /* Infer the desktop file name from the basename of the executable.
               This is not 100% reliable, but should work reasonably well in
               most cases. */
            return exec + ".desktop"
        }
    }

    visible: application.desktop_file != ""

    onClicked: {
        dashView.active = false
        application.activate()
    }

    icon: "image://icons/" + application.icon
    iconSourceSize.width: 128
    iconSourceSize.height: 128
}
