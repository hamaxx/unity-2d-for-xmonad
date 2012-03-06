/*
 * Copyright (C) 2010 Canonical, Ltd.
 *
 * Authors:
 *  Ugo Riboni <ugo.riboni@canonical.com>
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

#include "spreadview.h"

#include <QDesktopWidget>
#include <QApplication>
#include <QMouseEvent>

#include "screeninfo.h"

SpreadView::SpreadView(int screen)
: Unity2DDeclarativeView()
{
    m_screenInfo = new ScreenInfo(screen, this);
    connect(m_screenInfo, SIGNAL(availableGeometryChanged(QRect)), SLOT(fitToAvailableSpace()));

    fitToAvailableSpace();
}

void SpreadView::fitToAvailableSpace()
{
    QRect geometry = m_screenInfo->panelsFreeGeometry();
    setGeometry(geometry);
    setFixedSize(geometry.size());
}
