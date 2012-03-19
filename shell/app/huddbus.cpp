/*
 * Copyright (C) 2011 Canonical, Ltd.
 *
 * Authors:
 *  Michał Sawicz <michal.sawicz@canonical.com>
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
#include <shellmanager.h>

// Qt
#include <QtDBus/QDBusConnection>
#include <QGraphicsObject>

HUDDBus::HUDDBus(ShellManager* manager, QObject* parent)
: QObject(parent)
, m_manager(manager)
{
    connect(m_manager, SIGNAL(hudActiveChanged()), SLOT(onHudActiveChanged()));
    connect(m_manager, SIGNAL(hudScreenChanged(int)), SIGNAL(screenChanged(int)));

    new HUDAdaptor(this);
}

bool
HUDDBus::active() const
{
    return m_manager->hudActive();
}

void
HUDDBus::onHudActiveChanged()
{
    Q_EMIT activeChanged(active());
}

void
HUDDBus::setActive(bool hudActive)
{
    if (hudActive != active()) {
        m_manager->setHudActive(hudActive);
        Q_EMIT activeChanged(hudActive);
    }
}

int
HUDDBus::screen() const
{
    return m_manager->hudScreen();
}
