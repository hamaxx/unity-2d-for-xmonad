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
import gconf 1.0
/* Necessary to access the blended image provider and CacheEffect */
import Unity2d 1.0

Item {
    property string overlay_color
    property real overlay_alpha

    /* Avoid redraw at rendering */
    effect: CacheEffect {}

    GConfItem {
        id: primary_color
        key: "/desktop/gnome/background/primary_color"
    }

    GConfItem {
        id: picture_filename
        key: "/desktop/gnome/background/picture_filename"
    }

    GConfItem {
        id: picture_options
        key: "/desktop/gnome/background/picture_options"
    }

    Rectangle {
        Rectangle {
            anchors.fill: parent
            opacity: overlay_alpha
            color: overlay_color
        }

        anchors.fill: parent
        color: primary_color.value
    }

    Image {
        id: picture

        visible: picture_filename.value
        source: {
            if (!visible) return ""
            
            /* FIXME: Because /usr/share/backgrounds/warty-final-ubuntu.png is
                      actually a jpeg and Qt relies by default on the extension
                      that particular background fails to load. We workaround
                      it by having our own symlink with a 'jpg' extension.

               References:
               https://bugs.launchpad.net/ubuntu/+source/ubuntu-wallpapers/+bug/296538
               http://bugreports.qt.nokia.com/browse/QTBUG-7276
            */
            var filename = picture_filename.value
            if(filename == "/usr/share/backgrounds/warty-final-ubuntu.png")
                filename = engineBaseUrl + "artwork/warty-final-ubuntu.jpg"

            if(overlay_alpha > 0.0)
                return "image://blended/%1color=%2alpha=%3".arg(filename).arg(overlay_color).arg(overlay_alpha)
            else
                return filename
        }
        width: screen.geometry.width
        height: screen.geometry.height

        smooth: true
        x: screen.availableGeometry.x
        y: -screen.availableGeometry.y

        /* Possible modes are:
            - "wallpaper"
            - "centered" (NOT IMPLEMENTED)
            - "scaled"
            - "stretched"
            - "zoom"
            - "spanned" (NOT IMPLEMENTED)
        */
        fillMode: {
            if(picture_options.value == "wallpaper")
                return Image.Tile
            else if(picture_options.value == "scaled")
                return Image.PreserveAspectFit
            else if(picture_options.value == "stretched")
                return Image.Stretch
            else if(picture_options.value == "zoom")
                return Image.PreserveAspectCrop
            else return Image.PreserveAspectFit
        }
    }
}
