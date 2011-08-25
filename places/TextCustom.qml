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

/* Custom Text factorising the font style. */
Text {
    color: "#ffffff"
//    font.family: "UbuntuBeta"
    property string fontSize: "medium"
    font.pixelSize: {
        switch (fontSize) {
            case "small": return 13
            case "medium": return 15
            case "large": return 16
            case "x-large": return 20
            default: return 14
        }
    }
}
