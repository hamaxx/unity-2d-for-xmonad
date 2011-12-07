/*
 * This file is part of unity-2d
 *
 * Copyright 2011 Canonical Ltd.
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

HomeButton {
    property alias contentType: defaultApplication.contentType
    /* If the desktopFile property is set and points to an existing
     * application, the contentType property is ignored.
     */
    property alias desktopFile: defaultApplication.defaultDesktopFile

    GioDefaultApplication {
        id: defaultApplication
    }

    LauncherApplication {
        id: application
        desktop_file: defaultApplication.desktopFile
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
