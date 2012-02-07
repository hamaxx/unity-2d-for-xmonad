/*
 * Copyright (C) 2012 Canonical, Ltd.
 *
 * Authors:
 *  Gerry Boland <gerry.boland@canonical.com>
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

#include "huddbus.h"
#include "hudadaptor.h"

// Local
#include <shelldeclarativeview.h>

// Qt
#include <QtDBus/QDBusConnection>

static const char* HUD_DBUS_SERVICE = "com.canonical.Unity2d.Hud";
static const char* HUD_DBUS_OBJECT_PATH = "/Hud";

HudDBus::HudDBus(ShellDeclarativeView* view, QObject* parent)
: QObject(parent)
, m_view(view)
{
    connect(m_view, SIGNAL(hudActiveChanged(bool)), SIGNAL(activeChanged(bool)));
}

HudDBus::~HudDBus()
{
    QDBusConnection::sessionBus().unregisterService(HUD_DBUS_SERVICE);
}

bool
HudDBus::connectToBus()
{
    bool ok = QDBusConnection::sessionBus().registerService(HUD_DBUS_SERVICE);
    if (!ok) {
        return false;
    }
    new HudAdaptor(this);
    QDBusConnection::sessionBus().registerObject(HUD_DBUS_OBJECT_PATH, this);

    return true;
}

bool
HudDBus::active() const
{
    return m_view->hudActive();
}

void
HudDBus::setActive(bool active)
{
    m_view->setHudActive(active);
}

