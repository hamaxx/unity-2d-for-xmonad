/*
 * Copyright (C) 2010 Canonical, Ltd.
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
#include <QRegExp>

#include "windowslist.h"
#include "windowinfo.h"

#include "bamf-matcher.h"
#include "bamf-window.h"
#include "bamf-application.h"

WindowsList::WindowsList(QObject *parent) :
    QAbstractListModel(parent),
    m_loaded(false)
{
    QHash<int, QByteArray> roles;
    roles[WindowInfo::RoleWindowInfo] = "window";
    roles[WindowInfo::RoleDesktopFile] = "desktopFile";
    roles[WindowInfo::RoleWorkspace] = "workspace";
    setRoleNames(roles);
}

WindowsList::~WindowsList()
{
    qDeleteAll(m_windows);
}

int WindowsList::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)

    return m_windows.size();
}

QVariant WindowsList::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    WindowInfo *info = m_windows.at(index.row());
    switch (role) {
    case WindowInfo::RoleWindowInfo:
        return QVariant::fromValue(info);
    case WindowInfo::RoleDesktopFile:
        return QVariant::fromValue(info->desktopFile());
    case WindowInfo::RoleWorkspace:
        return QVariant::fromValue(info->workspace());
    default:
        qDebug() << "Requested invalid role (index" << role << ")";
        return QVariant();
    }
}

void WindowsList::load()
{
    if (m_loaded) {
        return;
    }

    BamfMatcher &matcher = BamfMatcher::get_default();
    QList<BamfApplication*> applications;

    /* List the windows of all the applications */
    BamfApplicationList *allapplications = matcher.applications();
    for (int i = 0; i < allapplications->size(); i++) {
         applications.append(allapplications->at(i));
    }

    BamfWindow* activeWindow = matcher.active_window();

    /* Build a list of windowInfos for windows that are 'user_visible' according to BAMF */
    QList<WindowInfo*> newWindows;

    Q_FOREACH (BamfApplication* application, applications) {
        if (!application->user_visible()) {
            continue;
        }

        BamfWindowList *bamfWindows = application->windows();
        for (int i = 0; i < bamfWindows->size(); i++) {
            BamfWindow* window = bamfWindows->at(i);
            if (!window->user_visible()) {
                continue;
            }

            WindowInfo *info = new WindowInfo(window->xid());
            newWindows.append(info);
        }
    }

    qDebug() << "Matched" << newWindows.count() << "Windows in" << applications.count() << "Applications";

    if (m_windows.count() > 0) {
        beginRemoveRows(QModelIndex(), 0, m_windows.count() - 1);
        qDeleteAll(m_windows);
        m_windows.clear();
        endRemoveRows();
    }

    if (newWindows.count() > 0) {
        beginInsertRows(QModelIndex(), 0, newWindows.count() - 1);
        m_windows.append(newWindows);
        endInsertRows();
    }

    m_loaded = true;

    Q_EMIT countChanged(m_windows.count());
}

void WindowsList::unload()
{
    beginRemoveRows(QModelIndex(), 0, m_windows.count() - 1);
    qDeleteAll(m_windows);
    m_windows.clear();
    endRemoveRows();

    m_loaded = false;

    Q_EMIT countChanged(m_windows.count());
}
