/*
 * This file is part of unity-2d
 *
 * Copyright 2011 Canonical Ltd.
 *
 * Authors:
 * - Alberto Mardegan <mardy@users.sourceforge.net>
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

// Self
#include "homebutton.h"

// Local
#include <debug_p.h>

// Qt
#include <QApplication>

HomeButton::HomeButton(QWidget *parent)
: QToolButton(parent)
{
    setAutoRaise(true);
    setIconSize(QSize(24, 24));
    QString themeIconName = QIcon::hasThemeIcon("start-here") ? "start-here" : "distributor-logo";
    setIcon(QIcon::fromTheme(themeIconName));
    setCheckable(true);

    setStyleSheet(
            "QToolButton { border: none; margin: 0; padding: 0; width: 61 }"
            "QToolButton:checked, QToolButton:pressed {"
            // Use border-image here, not background-image, because bfb_bg_active.png is 56px wide
            "     border-image: url(theme:/bfb_bg_active.png);"
            "}"
    );
}

void HomeButton::mousePressEvent(QMouseEvent *event)
{
    if (lastClickTime.isValid() &&
        lastClickTime.elapsed() < QApplication::doubleClickInterval()) {
        /* ignore this click */
        return;
    }

    QToolButton::mousePressEvent(event);
    lastClickTime.restart();
}

#include "homebutton.moc"
