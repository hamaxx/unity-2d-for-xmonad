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

#include "workspaces.h"

#include "config.h"

#include <QDBusInterface>
#include <QDBusReply>

// libunity-2d
#include <unity2dtr.h>
#include <debug_p.h>

Workspaces::Workspaces()
{
    setShortcutKey(Qt::Key_S);
}

Workspaces::Workspaces(const Workspaces& other)
{
}

Workspaces::~Workspaces()
{
}

bool
Workspaces::active() const
{
    return false;
}

bool
Workspaces::running() const
{
    return false;
}

int
Workspaces::windowCount() const
{
    return 0;
}

bool
Workspaces::urgent() const
{
    return false;
}

QString
Workspaces::name() const
{
    return u2dTr("Workspaces");
}

QString
Workspaces::icon() const
{
    return QString("unity-icon-theme/workspace-switcher");
}

bool
Workspaces::launching() const
{
    return false;
}

void
Workspaces::activate()
{
    QDBusInterface iface("com.canonical.Unity2d.Spread", "/Spread",
                         "com.canonical.Unity2d.Spread");
    QDBusReply<bool> isShown = iface.call("IsShown");
    if (isShown.isValid()) {
        if (isShown.value() == true) {
            iface.asyncCall("FilterByApplication", QString());
        } else {
            iface.asyncCall("ShowAllWorkspaces", QString());
        }
    } else {
        UQ_WARNING << "Failed to get property IsShown on com.canonical.Unity2d.Spread";
    }
}

void
Workspaces::createMenuActions()
{
    return;
}

WorkspacesList::WorkspacesList(QObject* parent) :
    QAbstractListModel(parent)
{
    m_workspaces = new Workspaces();
}

WorkspacesList::~WorkspacesList()
{
    delete m_workspaces;
}

int
WorkspacesList::rowCount(const QModelIndex& parent) const
{
    Q_UNUSED(parent)

    return 1;
}

QVariant
WorkspacesList::data(const QModelIndex& index, int role) const
{
    Q_UNUSED(role)

    if (!index.isValid())
        return QVariant();

    return QVariant::fromValue(m_workspaces);
}

