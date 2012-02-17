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

FilterCheckoption {
    /*TODO: This is a workaround for the unity-core bug
      Files & Folders-> LastModified should have 3 columns not 2
      https://bugs.launchpad.net/unity-lens-files/+bug/928208
      We should remove the following block as soon as
      unity-core fixes the issue
    */
    grid_columns : {
        if (filterModel.id === "modified") {
            return 3
        } else {
            return 2
        }
    }
}
