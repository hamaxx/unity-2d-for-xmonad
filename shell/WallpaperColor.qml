/*
 * This file is part of unity-2d
 *
 * Copyright 2012 Canonical Ltd.
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
import QConf 1.0
import Unity2d 1.0

/* Save wallpaper average color in Unity's configuration key 'averageBgColor'
   FIXME: the case where the wallpaper is a slideshow is not yet supported
*/
/* FIXME: change of wallpaper is not taken into account live because of a bug
   in dconf-dbus. Patch is available and needs to be pushed in Ubuntu.
   Ref.: https://bugzilla.gnome.org/show_bug.cgi?id=669915
*/
Item {
    QConf {
        id: desktopBackground
        schema: "org.gnome.desktop.background"
    }

    PercentCoder {
        id: backgroundFilename
        encoded: desktopBackground.pictureUri
    }

    ImageUtilities {
        id: imageUtilities
        source: {
            /* FIXME: Because /usr/share/backgrounds/warty-final-ubuntu.png is
                      actually a jpeg and Qt relies by default on the extension
                      that particular background fails to load. We workaround
                      it by having our own symlink with a 'jpg' extension.

               References:
               https://bugs.launchpad.net/ubuntu/+source/ubuntu-wallpapers/+bug/296538
               http://bugreports.qt.nokia.com/browse/QTBUG-7276
            */
            var filename = backgroundFilename.unencoded /* path is urlencoded */
            if(filename == "file:///usr/share/backgrounds/warty-final-ubuntu.png")
                filename = "/usr/share/unity-2d/warty-final-ubuntu.jpg"
            return filename
        }
    }

    Binding {
        target: unityConfiguration
        property: "averageBgColor"
        value: desktopBackground.pictureUri != "" ? imageUtilities.averageColor : desktopBackground.primaryColor
    }
}
