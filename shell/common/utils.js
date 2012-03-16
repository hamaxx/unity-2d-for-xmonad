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

.pragma library

/* Convert strings like "one-two-three" to "OneTwoThree" */
function convertToCamelCase( name ) {
    var chunksArray = name.split('-')
    var camelName = ''
    for (var i=0; i<chunksArray.length; i++){
        camelName = camelName + chunksArray[i].charAt(0).toUpperCase() + chunksArray[i].slice(1);
    }
    return camelName
}

function clamp(x, min, max) {
    return Math.max(Math.min(x, max), min)
}

function hashEmpty(hash) {
    for (var key in hash) return false;
    return true
}

function isLeftToRight() {
    return Qt.application.layoutDirection == Qt.LeftToRight
}

function isRightToLeft() {
    return Qt.application.layoutDirection == Qt.RightToLeft
}

function switchLeftRightKeys(key) {
    if (isRightToLeft()) {
        switch (key) {
        case Qt.Key_Right:
            return Qt.Key_Left
        case Qt.Key_Left:
            return Qt.Key_Right
        default:
            return key
        }
    }
    return key
}
