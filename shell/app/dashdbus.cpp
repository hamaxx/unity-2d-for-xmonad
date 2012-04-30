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

#include "dashdbus.h"
#include "dashadaptor.h"

// Local
#include <shelldeclarativeview.h>
#include <shellmanager.h>

// Qt
#include <QtDBus/QDBusConnection>
#include <QGraphicsObject>

DashDBus::DashDBus(ShellManager* manager, QObject* parent)
: QObject(parent)
, m_manager(manager)
{
    connect(m_manager, SIGNAL(dashActiveChanged(bool)), SIGNAL(activeChanged(bool)));
    connect(m_manager, SIGNAL(dashAlwaysFullScreenChanged(bool)), SIGNAL(alwaysFullScreenChanged(bool)));
    connect(m_manager, SIGNAL(dashActiveLensChanged(QString)), SIGNAL(activeLensChanged(QString)));

    connect(m_manager, SIGNAL(dashScreenChanged(int)), SIGNAL(screenChanged(int)));

    new DashAdaptor(this);
}

void
DashDBus::activateHome()
{
    Q_EMIT m_manager->dashActivateHome();
}

void
DashDBus::activateLens(const QString& lensId)
{
    Q_EMIT m_manager->dashActivateLens(lensId);
}

bool
DashDBus::active() const
{
    return m_manager->dashActive();
}

void
DashDBus::setActive(bool active)
{
    m_manager->setDashActive(active);
    if (!active) {
        m_manager->forceDeactivateShell(static_cast<ShellDeclarativeView*>(m_manager->dashShell()));
    }
}

bool
DashDBus::alwaysFullScreen() const
{
    return m_manager->dashAlwaysFullScreen();
}

QString
DashDBus::activeLens() const
{
    return m_manager->dashActiveLens();
}

void
DashDBus::setActiveLens(QString activeLens)
{
    m_manager->setDashActiveLens(activeLens);
}

int
DashDBus::screen() const
{
    return m_manager->dashScreen();
}
