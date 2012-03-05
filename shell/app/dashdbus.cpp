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

// Qt
#include <QtDBus/QDBusConnection>
#include <QGraphicsObject>

DashDBus::DashDBus(ShellDeclarativeView* view, QObject* parent)
: QObject(parent)
, m_view(view)
{
    connect(m_view, SIGNAL(dashActiveChanged(bool)), SIGNAL(activeChanged(bool)));
    connect(m_view, SIGNAL(dashAlwaysFullScreenChanged(bool)), SIGNAL(alwaysFullScreenChanged(bool)));
    connect(m_view, SIGNAL(activeLensChanged(QString)), SIGNAL(activeLensChanged(QString)));

    new DashAdaptor(this);
}

void
DashDBus::activateHome()
{
    Q_EMIT m_view->activateHome();
}

void
DashDBus::activateLens(const QString& lensId)
{
    Q_EMIT m_view->activateLens(lensId);
}

bool
DashDBus::active() const
{
    return m_view->dashActive();
}

void
DashDBus::setActive(bool active)
{
    m_view->setDashActive(active);
}

bool
DashDBus::alwaysFullScreen() const
{
    return m_view->dashAlwaysFullScreen();
}

QString
DashDBus::activeLens() const
{
    return m_view->activeLens();
}

void
DashDBus::setActiveLens(QString activeLens)
{
    m_view->setActiveLens(activeLens);
}
