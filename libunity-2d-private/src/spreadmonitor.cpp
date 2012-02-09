/*
 * Copyright (C) 2011 Canonical, Ltd.
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

// local
#include "abstractdbusservicemonitor.h"
#include "spreadmonitor.h"

// QT
#include <QDBusConnection>
#include <QDBusConnectionInterface>
#include <QDBusReply>

SpreadMonitor::SpreadMonitor(QObject *parent)
    : AbstractDBusServiceMonitor("com.canonical.Unity2d.Spread", "/Spread",
                                "com.canonical.Unity2d.Spread", parent)
{
    connect(this, SIGNAL(serviceAvailableChanged(bool)), SLOT(onServiceAvailableChanged(bool)));

    if (serviceAvailable()) {
        onServiceAvailableChanged(true);
    }
}

void SpreadMonitor::onServiceAvailableChanged(bool available)
{
    if (available) {
        connect(dbusInterface(), SIGNAL(IsShownChanged(bool)), SIGNAL(shownChanged(bool)));

        Q_EMIT shownChanged(shown());
    } else {
        Q_EMIT shownChanged(false);
    }
}

bool SpreadMonitor::shown() const
{
    if (dbusInterface() == 0) {
        return false;
    } else {
        QDBusReply<bool> result = dbusInterface()->call("IsShown");
        return result.isValid() && result.value() == true;
    }
}

#include "spreadmonitor.moc"
