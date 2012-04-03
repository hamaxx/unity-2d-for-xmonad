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

Item {
    id: iconTile

    property bool activeFocus: false
    property bool backgroundFromIcon: true
    property string source: "image://icons/unknown"
    property alias backgroundOpacity: tileBackground.opacity

    /* For icons that do not follow usual tile design, can specify custom tile background,
       custom tile background when tile selected, and custom shine image for on top */
    property string selectedTileBackgroundImage: ""
    property string tileBackgroundImage: ""
    property string tileShineImage: ""

    property color defaultBackgroundColor: "#333333"
    property color selectedBackgroundColor: "#dddddd"

    /* This is the image providing the background image. The color blended
       with this image is obtained from the color of the icon when it's loaded.
       While the application is launching, this will fade out and in. */
    Image {
        id: tileBackground
        objectName: "tileBackground"
        property color color: defaultBackgroundColor
        anchors.fill: parent
        smooth: true

        source: {
            if (selectedTileBackgroundImage != "" && declarativeView.focus && iconTile.activeFocus) {
                return selectedTileBackgroundImage
            } else if (tileBackgroundImage != "") {
                return tileBackgroundImage
            }

            var actualColor = declarativeView.focus && iconTile.activeFocus ? selectedBackgroundColor : color
            return "image://blended/%1color=%2alpha=%3"
                .arg("common/artwork/round_corner_54x54.png")
                .arg(actualColor.toString().replace("#", ""))
                .arg(1.0)
        }
    }

    /* This is just the main icon of the tile */
    Image {
        id: icon
        objectName: "icon"
        anchors.centerIn: parent
        smooth: true

        sourceSize.width: 48
        sourceSize.height: 48

        /* Whenever one of the parameters used in calculating the background color of
           the icon changes, recalculate its value */
        onWidthChanged: updateColors()
        onHeightChanged: updateColors()
        onSourceChanged: updateColors()
        onStatusChanged: if (status == Image.Error) source = "image://icons/unknown"

        function updateColors() {
            if (!backgroundFromIcon) return;

            var colors = iconUtilities.getColorsFromIcon(icon.source, icon.sourceSize)
            if (colors && colors.length > 0) tileBackground.color = colors[0]
        }
    }

    /* This just adds some shiny effect to the tile */
    Image {
        id: tileShine
        anchors.fill: parent
        smooth: true

        source: (tileShineImage != "") ? tileShineImage : "artwork/round_shine_54x54.png"
    }

    onSourceChanged: icon.source = iconTile.source
}
