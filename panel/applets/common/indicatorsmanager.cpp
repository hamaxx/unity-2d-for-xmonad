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
#include "indicatorsmanager.h"

// Local
#include <debug_p.h>
#include <indicatorentrywidget.h>

// Qt
#include <QApplication>
#include <QTimer>
#include <QX11Info>

// UnityCore
#include <UnityCore/UnityCore.h>

// X11
#include <X11/Xlib.h>

using namespace unity::indicator;

IndicatorsManager::IndicatorsManager(QObject* parent)
: QObject(parent)
, m_indicators(new DBusIndicators)
, m_geometrySyncTimer(new QTimer(this))
{
    m_geometrySyncTimer->setInterval(0);
    m_geometrySyncTimer->setSingleShot(true);
    connect(m_geometrySyncTimer, SIGNAL(timeout()), SLOT(syncGeometries()));

    m_indicators->on_entry_show_menu.connect(
        sigc::mem_fun(this, &IndicatorsManager::onEntryShowMenu)
        );

    m_indicators->on_menu_pointer_moved.connect(
        sigc::mem_fun(this, &IndicatorsManager::onMenuPointerMoved)
        );

    m_indicators->on_entry_activate_request.connect(
        sigc::mem_fun(this, &IndicatorsManager::onEntryActivateRequest)
        );

    m_indicators->on_synced.connect(
        sigc::mem_fun(this, &IndicatorsManager::onSynced)
        );
}

unity::indicator::DBusIndicators::Ptr IndicatorsManager::indicators() const
{
    return m_indicators;
}

void IndicatorsManager::onEntryShowMenu(const std::string& /*entryId*/, int posX, int posY, int /*timestamp*/, int /*button*/)
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

void IndicatorsManager::onMenuPointerMoved(int posX, int posY)
{
    QWidget* widget = QApplication::widgetAt(posX, posY);
    IndicatorEntryWidget* entryWidget = qobject_cast<IndicatorEntryWidget*>(widget);
    if (!entryWidget) {
        return;
    }
    entryWidget->showMenu(Qt::NoButton);
}

void IndicatorsManager::onEntryActivateRequest(const std::string& entryId)
{
    if (entryId.empty()) {
        return;
    }
    IndicatorEntryWidget* widget = m_widgetForEntryId.value(entryId);
    if (!widget) {
        UQ_WARNING << "Could not find a widget for IndicatorEntry with id" << QString::fromStdString(entryId);
        return;
    }
    widget->showMenu(Qt::NoButton);
}

void IndicatorsManager::onSynced()
{
    QMetaObject::invokeMethod(m_geometrySyncTimer, "start", Qt::QueuedConnection);
}

void IndicatorsManager::addIndicatorEntryWidget(IndicatorEntryWidget* widget)
{
    m_widgetForEntryId.insert(widget->entry()->id(), widget);
    widget->installEventFilter(this);
}

bool IndicatorsManager::eventFilter(QObject*, QEvent* event)
{
    switch (event->type()) {
    case QEvent::Show:
    case QEvent::Hide:
    case QEvent::Move:
    case QEvent::Resize:
        m_geometrySyncTimer->start();
        break;
    default:
        break;
    }
    return false;
}

void IndicatorsManager::syncGeometries()
{
    EntryLocationMap locations;
    Q_FOREACH(IndicatorEntryWidget* widget, m_widgetForEntryId) {
        if (!widget->isVisible()) {
            continue;
        }
        Entry::Ptr entry = widget->entry();
        if (entry->IsUnused()) {
            continue;
        }
        QPoint topLeft = widget->mapToGlobal(QPoint(0, 0));
        nux::Rect rect(topLeft.x(), topLeft.y(), widget->width(), widget->height());
        locations[widget->entry()->id()] = rect;
    }
    m_indicators->SyncGeometries("Panel", locations);
}

#include "indicatorsmanager.moc"
