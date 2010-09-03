/*
 * Copyright (C) 2010 Canonical, Ltd.
 *
 * Authors:
 *  Olivier Tilloy <olivier.tilloy@canonical.com>
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

#include "launcherview.h"

#include <QApplication>
#include <QDesktopWidget>
#include <QX11Info>

#include <X11/Xlib.h>
#include <X11/Xatom.h>

LauncherView::LauncherView() :
    QDeclarativeView(), m_resizing(false), m_reserved(false)
{
}

void
LauncherView::workAreaResized(int screen)
{
    if (m_resizing)
    {
        /* FIXME: this is a hack to avoid infinite recursion: reserving space
           at the left of the screen triggers the emission of the
           workAreaResized signal… */
        m_resizing = false;
        return;
    }

    QDesktopWidget* desktop = QApplication::desktop();
    if (screen == desktop->screenNumber(this))
    {
        const QRect screen = desktop->screenGeometry(this);
        const QRect available = desktop->availableGeometry(this);
        this->resize(60, available.height());
        uint left = available.x();
        /* This assumes that we are the only panel on the left of the screen */
        if (m_reserved) left -= this->size().width();
        this->move(left, available.y());

        m_resizing = true;

        /* Reserve space at the left edge of the screen (the launcher is a panel) */
        Atom atom = XInternAtom(QX11Info::display(), "_NET_WM_STRUT_PARTIAL", False);
        uint struts[12] = {left + this->size().width(), 0, 0, 0,
                           available.y(), this->size().height(), 0, 0,
                           0, 0, 0, 0};
        XChangeProperty(QX11Info::display(), this->effectiveWinId(), atom,
                        XA_CARDINAL, 32, PropModeReplace,
                        (unsigned char *) &struts, 12);
        m_reserved = true;
    }
}

