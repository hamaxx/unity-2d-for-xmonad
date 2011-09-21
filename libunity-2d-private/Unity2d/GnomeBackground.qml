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
import QConf 1.0
/* Necessary to access the blended image provider and CacheEffect */
import Unity2d 1.0

Item {
    property string overlay_color
    property real overlay_alpha
    
    /* Avoid redraw at rendering */
    CacheEffect {
        id: cacheEffect
    }
    property bool cached: true
    effect: (cached) ? cacheEffect : null

    QConf {
        id: desktopBackground
        schema: "org.gnome.desktop.background"
    }

    PercentCoder {
        id: backgroundFilename
        encoded: desktopBackground.pictureUri
    }

    Rectangle {
        Rectangle {
            anchors.fill: parent
            opacity: overlay_alpha
            color: overlay_color
        }

        anchors.fill: parent
        color: desktopBackground.primaryColor
    }

    Image {
        id: picture

        visible: desktopBackground.pictureUri
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
            var filename = backgroundFilename.unencoded /* path is urlencoded */
            if(filename == "file:///usr/share/backgrounds/warty-final-ubuntu.png")
                filename = "/usr/share/unity-2d/warty-final-ubuntu.jpg"

            if(overlay_alpha > 0.0)
                return "image://blended/%1color=%2alpha=%3".arg(filename).arg(overlay_color).arg(overlay_alpha)
            else
                return filename
        }
        width: screen.geometry.width
        height: screen.geometry.height

        smooth: true
        x: screen.availableGeometry.x - declarativeView.globalPosition.x
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
            if(desktopBackground.pictureOptions== "wallpaper")
                return Image.Tile
            else if(desktopBackground.pictureOptions == "scaled")
                return Image.PreserveAspectFit
            else if(desktopBackground.pictureOptions == "stretched")
                return Image.Stretch
            else if(desktopBackground.pictureOptions == "zoom")
                return Image.PreserveAspectCrop
            else return Image.PreserveAspectFit
        }
    }
}
