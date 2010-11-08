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
LauncherItem::showTooltip(int y)
{
    if (m_menu->isVisible())
        return;

    /* Escaping ampersands so that they are not considered as keyboard
       accelerators. */
    m_menu->setTitle(name().replace("&", "&&"));

    QDesktopWidget* desktop = QApplication::desktop();
    const QRect available = desktop->availableGeometry(m_menu);
    m_menu->move(available.x(), y + available.y() - m_menu->sizeHint().height() / 2);
    m_menu->show();
}

void
LauncherItem::showMenu()
{
    /* Is the menu already unfolded? */
    if (m_menu->actions().size() > 1)
        return;

    m_menu->addSeparator();
    createMenuActions();
}

void
LauncherItem::hideMenu(bool force)
{
    if (!force)
    {
        /* Should the menu really be hidden? This trick discards invalid calls
           to hideMenu() from Launcher.qml: the onExited event shouldn't cause
           the menu to hide if the mouse cursor has left the button to enter
           the menu, otherwise the user never gets a chance to use the menu.
           This logic should live in the calling code, but I'm not sure how to
           achieve that in QML… */
        bool unfolded = (m_menu->actions().size() > 1);
        if (unfolded)
        {
            if (m_menu->geometry().contains(QCursor::pos()))
                return;
        }
    }

    m_menu->hide();
    m_menu->clear();
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
        hideMenu(true);
        return true;
    }
    else
    {
        /* Standard event processing */
        return QObject::eventFilter(obj, event);
    }
}

