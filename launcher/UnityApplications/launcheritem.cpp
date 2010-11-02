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

#include "launcheritem.h"

#include <QApplication>
#include <QDesktopWidget>
#include <QPoint>
#include <QCursor>

LauncherItem::LauncherItem()
{
    m_menu = new LauncherContextualMenu;

    /* Catch events sent to m_menu */
    m_menu->installEventFilter(this);
}

LauncherItem::~LauncherItem()
{
    delete m_menu;
}

void
LauncherItem::show_tooltip(int y)
{
    if (m_menu->isVisible())
        return;

    QDesktopWidget* desktop = QApplication::desktop();
    const QRect available = desktop->availableGeometry(m_menu);
    m_menu->move(available.x(), y + available.y() - m_menu->sizeHint().height() / 2);
    m_menu->show();
    /* Set the title after showing so that the width is correctly updated. */
    m_menu->setTitle(name());
}

void
LauncherItem::show_menu()
{
    /* Is the menu already unfolded? */
    if (m_menu->actions().size() > 1)
        return;

    really_show_menu();
}

void
LauncherItem::hide_menu()
{
    /* Should the menu really be hidden? */
    bool unfolded = (m_menu->actions().size() > 1);
    if (unfolded)
    {
        QDesktopWidget* desktop = QApplication::desktop();
        const QRect available = desktop->availableGeometry(m_menu);
        QPoint cursor = QCursor::pos();
        if (cursor.x() >= available.x())
            return;
    }

    really_hide_menu();
}

bool
LauncherItem::eventFilter(QObject* obj, QEvent* event)
{
    if (event->type() == QEvent::Leave)
    {
        /* Since our contextual menu is not a modal popup, it doesn’t capture
           all events, and thus doesn’t know when e.g. the mouse was clicked
           outside of its window (which should close it).
           Intercepting the Leave event is a cheap workaround: hide the menu
           when the cursor leaves it. This is not the same behaviour as in
           unity, but it will do for now… */
        really_hide_menu();
        return true;
    }
    else
    {
        /* Standard event processing */
        return QObject::eventFilter(obj, event);
    }
}

