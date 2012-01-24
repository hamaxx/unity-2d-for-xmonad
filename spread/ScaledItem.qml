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

Item {
    property variant target
    property int itemWidth
    property int itemHeight

    /* Scale down to fit target while preserving the aspect ratio of the item.
       Never scale up the item. */
    property double targetAspectRatio: target.width / target.height
    property double itemAspectRatio: itemWidth / itemHeight
    property bool isHorizontal: itemAspectRatio >= targetAspectRatio
    property int maxWidth: Math.min(itemWidth, target.width)
    property int maxHeight: Math.min(itemHeight, target.height)

    width: isHorizontal ? maxWidth : maxHeight * itemAspectRatio
    height: !isHorizontal ? maxHeight : maxWidth / itemAspectRatio
    x: target.x + (target.width - width) / 2
    y: target.y + (target.height - height) / 2
}
