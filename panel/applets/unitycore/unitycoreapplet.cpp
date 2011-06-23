/*
 * This file is part of unity-2d
 *
 * Copyright 2011 Canonical Ltd.
 *
 * Authors:
 * - Aurélien Gâteau <aurelien.gateau@canonical.com>
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
#include "unitycoreapplet.h"

// Local
#include <debug_p.h>
#include <indicatorentrywidget.h>
#include <indicatorwidget.h>

// Qt
#include <QApplication>
#include <QHBoxLayout>
#include <QX11Info>

// X11
#include <X11/Xlib.h>

using namespace unity::indicator;

UnityCoreApplet::UnityCoreApplet()
: m_indicators(new DBusIndicators())
{
    QHBoxLayout* layout = new QHBoxLayout(this);
    layout->setMargin(0);
    layout->setSpacing(0);

    m_indicators->on_object_added.connect(
        sigc::mem_fun(this, &UnityCoreApplet::onObjectAdded)
        );

    m_indicators->on_entry_show_menu.connect(
        sigc::mem_fun(this, &UnityCoreApplet::onEntryShowMenu)
        );

    m_indicators->on_menu_pointer_moved.connect(
        sigc::mem_fun(this, &UnityCoreApplet::onMenuPointerMoved)
        );
}

void UnityCoreApplet::onObjectAdded(Indicator::Ptr const& indicator)
{
    QString name = QString::fromStdString(indicator->name());
    if (name == "libappmenu.so") {
        // Skip appmenu for now
        return;
    }
    IndicatorWidget* widget = new IndicatorWidget(indicator);
    layout()->addWidget(widget);
}

void UnityCoreApplet::onEntryShowMenu(const std::string& /*entryId*/, int posX, int posY, int /*timestamp*/, int /*button*/)
{
    // Copied from plugins/unityshell/src/PanelView.cpp, in OnEntryShowMenu()
    // Without this code, menus cannot be shown from mousePressEvent() (but can
    // be shown from mouseReleaseEvent())
    /*
    Neil explanation:
    On button down, X automatically gives Qt a passive grab on the mouse this
    means that, if the panel service tries to grab the pointer to show the menu
    (gtk does this automatically), it fails and the menu can't show.
    We connect to the on_entry_show_menu signal, which is emitted before
    DBusIndicators does anything else, and just break the grab.
    */
    Display* display = QX11Info::display();
    XUngrabPointer(display, CurrentTime);
    XFlush(display);

    XButtonEvent event = {
        ButtonRelease,
        0,
        False,
        display,
        0,
        0,
        0,
        CurrentTime,
        posX, posY,
        posX, posY,
        0,
        Button1,
        True
    };
    qApp->x11ProcessEvent(reinterpret_cast<XEvent*>(&event));
}

void UnityCoreApplet::onMenuPointerMoved(int posX, int posY)
{
    QWidget* widget = QApplication::widgetAt(posX, posY);
    IndicatorEntryWidget* entryWidget = qobject_cast<IndicatorEntryWidget*>(widget);
    if (!entryWidget) {
        return;
    }
    entryWidget->showMenu();
}

#include "unitycoreapplet.moc"
