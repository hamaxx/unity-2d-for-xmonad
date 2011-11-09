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
