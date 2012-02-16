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

/* BackgroundRevision object to help update background only when wanted.
   It manages a global variable that is used to control when a new snapshot
   of the root window is needed. See Background.qml for more information */
function BackgroundRevision() {
    this.stamp = 0
}

BackgroundRevision.prototype.update = function() {
    this.stamp += 1
}

BackgroundRevision.prototype.getStamp = function() {
    return this.stamp
}

var background = new BackgroundRevision
