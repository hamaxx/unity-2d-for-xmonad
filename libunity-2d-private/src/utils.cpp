/*
 * Copyright (C) 2011 Canonical, Ltd.
 *
 * Authors:
 *  Haggai Eran <haggai.eran@gmail.com>
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

#include "utils.h"

#include <QApplication>

/* If using a right-to-left locale, switch the left and right keys. */
int switchLeftRightKeys(int key)
{
    if (QApplication::isRightToLeft()) {
        switch(key) {
            case Qt::Key_Right:
                return Qt::Key_Left;
            case Qt::Key_Left:
                return Qt::Key_Right;
            default:
                return key;
        }
    }
    return key;
}
