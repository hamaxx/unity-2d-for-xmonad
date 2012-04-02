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
                                "com.canonical.Unity2d.Spread", parent),
      m_shown(false)
{
    connect(this, SIGNAL(serviceAvailableChanged(bool)), SLOT(onServiceAvailableChanged(bool)));

    if (serviceAvailable()) {
        onServiceAvailableChanged(true);
    }
}

void SpreadMonitor::onServiceAvailableChanged(bool available)
{
    if (available) {
        connect(dbusInterface(), SIGNAL(IsShownChanged(bool)), SLOT(internalSetShown(bool)));

        /* In SpreadMonitor::shown() we assume shown is false if the service is not
           available therefore there is no need to emit shownChanged if it is still false */
        if (dbusShown()) {
            internalSetShown(true);
        }
    } else {
        /* At this point we cannot know for sure that the spread was shown so
           we emit the changed signal unconditionally */
        internalSetShown(false);
    }
}

bool SpreadMonitor::shown() const
{
    return m_shown;
}

void SpreadMonitor::internalSetShown(bool shown)
{
    if (shown != m_shown) {
        m_shown = shown;
        Q_EMIT shownChanged(m_shown);
    }
}

bool SpreadMonitor::dbusShown() const
{
    if (dbusInterface() == 0) {
        return false;
    } else {
        QDBusReply<bool> result = dbusInterface()->call("IsShown");
        return result.isValid() && result.value() == true;
    }
}

#include "spreadmonitor.moc"
