/*
 * Copyright (C) 2010 Canonical, Ltd.
 *
 * Authors:
 *  Ugo Riboni <ugo.riboni@canonical.com>
 *  Florian Boucault <florian.boucault@canonical.com>
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

#include <QDebug>

#include "spreadcontrol.h"
#include "spreadadaptor.h"

static const char* DBUS_SERVICE = "com.canonical.Unity2d.Spread";
static const char* DBUS_OBJECT_PATH = "/Spread";

SpreadControl::SpreadControl(QObject *parent) :
    QObject(parent), m_isShown(false)
{
}

void
SpreadControl::setIsShown(bool isShown)
{
    if (isShown != m_isShown) {
        m_isShown = isShown;
        Q_EMIT IsShownChanged(isShown);
    }
}

SpreadControl::~SpreadControl()
{
    QDBusConnection::sessionBus().unregisterService(DBUS_SERVICE);
}

bool SpreadControl::connectToBus()
{
    bool ok = QDBusConnection::sessionBus().registerService(DBUS_SERVICE);
    if (!ok) {
        return false;
    }
    new SpreadAdaptor(this);
    QDBusConnection::sessionBus().registerObject(DBUS_OBJECT_PATH, this);

    return true;
}

void SpreadControl::ShowAllWorkspaces(QString applicationDesktopFile)
{
    Q_EMIT showAllWorkspaces(applicationDesktopFile);
}

void SpreadControl::ShowCurrentWorkspace(QString applicationDesktopFile)
{
    Q_EMIT showCurrentWorkspace(applicationDesktopFile);
}

void SpreadControl::FilterByApplication(QString applicationDesktopFile)
{
    Q_EMIT filterByApplication(applicationDesktopFile);
}

void SpreadControl::Hide()
{
    Q_EMIT hide();
}
