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

#include <QRegExp>
#include <QApplication>
#include <QWidget>
#include <QList>

#include <debug_p.h>
#include "windowslist.h"
#include "windowinfo.h"

#include "bamf-matcher.h"
#include "bamf-window.h"
#include "bamf-application.h"
#include "bamf-view.h"

WindowsList::WindowsList(QObject *parent) :
    QAbstractListModel(parent)
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
        UQ_DEBUG << "Requested invalid role (index" << role << ")";
        return QVariant();
    }
}

void WindowsList::load()
{
    BamfMatcher &matcher = BamfMatcher::get_default();
    connect(&matcher, SIGNAL(ViewOpened(BamfView*)), SLOT(addWindow(BamfView*)));
    connect(&matcher, SIGNAL(ViewClosed(BamfView*)), SLOT(removeWindow(BamfView*)));

    if (m_windows.count() > 0) {
        beginRemoveRows(QModelIndex(), 0, m_windows.count() - 1);
        qDeleteAll(m_windows);
        m_windows.clear();
        endRemoveRows();
    }

    QList<BamfApplication*> applications;

    /* List the windows of all the applications */
    BamfApplicationList *allapplications = matcher.applications();
    for (int i = 0; i < allapplications->size(); i++) {
         applications.append(allapplications->at(i));
    }

    /* Add to the list only WindowInfo for windows that are
      'user_visible' according to BAMF */
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
            connect(info, SIGNAL(workspaceChanged(int)), SLOT(updateWorkspaceRole(int)));

            /* It may seem less efficient to add items one by one instead of adding them
               to a list and appending the entire list at the end and notify only one
               size change.
               However doing it that way for some reason prevents the GridView connected
               to this model from emitting *any* onAdd signal, and the logic in Spread
               relies on these signals being reliably emitted. */
            beginInsertRows(QModelIndex(), m_windows.count(), m_windows.count());
            m_windows.append(info);
            endInsertRows();
        }
    }
}

void WindowsList::unload()
{
    BamfMatcher &matcher = BamfMatcher::get_default();
    matcher.disconnect(this, SLOT(addWindow(BamfView*)));
    matcher.disconnect(this, SLOT(removeWindow(BamfView*)));

    beginRemoveRows(QModelIndex(), 0, m_windows.count() - 1);
    qDeleteAll(m_windows);
    m_windows.clear();
    endRemoveRows();
}

void WindowsList::addWindow(BamfView *view)
{
    BamfWindow *window = qobject_cast<BamfWindow*>(view);
    if (window == NULL) {
        /* It is common for this to be null since Bamf sends
           us also one ViewOpened with BamfApplication* for the
           first window opened of each application. */
        return;
    }

    if (window->xid() == 0) {
        UQ_WARNING << "Received ViewOpened but window's xid is zero";
        return;
    }

    /* Prevent adding ourselves to the windows in the model */
    QWidget *ownWindow = QApplication::activeWindow();
    if (ownWindow != NULL && ownWindow->winId() == window->xid()) {
        return;
    }

    /* Prevent adding windows that the user sholdn't be able to
       manipulate in the switcher (i.e. the dash) */
    if (!window->user_visible()) {
        return;
    }

    WindowInfo *info = new WindowInfo(window->xid());
    connect(info, SIGNAL(workspaceChanged(int)), SLOT(updateWorkspaceRole(int)));

    beginInsertRows(QModelIndex(), m_windows.count(), m_windows.count());
    m_windows.append(info);
    endInsertRows();
}

void WindowsList::removeWindow(BamfView *view)
{
    BamfWindow *window = qobject_cast<BamfWindow*>(view);
    if (window == NULL) {
        /* It is common for this to be null since Bamf sends
           us also one ViewOpened with BamfApplication* for the
           last window closed of each application. */
        return;
    }

    /* The BamfMatcher::ViewClosed signal is emitted after the
       window is already gone. This means that it's not possible to
       retrieve the XID from the BamfWindow to find the window itself
       in the list.
       To workaround this, we compare directly the BamfWindow pointer
       to the one in the WindowInfo.
    */
    for (int i = 0; i < m_windows.length(); i++) {
        if (m_windows.at(i)->isSameBamfWindow(window)) {
            beginRemoveRows(QModelIndex(), i, i);
            delete m_windows.takeAt(i);
            endRemoveRows();
            return;
        }
    }
}

/* When the window is moved to another workspace, we need to have the list
   notify that something changed in the item corresponding to the window.
   If we don't do this, the proxy models will not refresh and the window
   won't be moved to the other workspace */
void WindowsList::updateWorkspaceRole(int workspace)
{
    Q_UNUSED(workspace);

    WindowInfo *window = qobject_cast<WindowInfo*>(sender());
    if (window != NULL) {
        int row = m_windows.indexOf(window);
        if (row != -1) {
            QModelIndex changedItem = index(row);
            Q_EMIT dataChanged(changedItem, changedItem);
        }
    }
}

bool WindowsList::removeRows(int row, int count, const QModelIndex& parent)
{
    if (row < 0 || row >= m_windows.count() || count <= 0) {
        return false;
    }
    count = qMin(count, m_windows.count() - row);

    beginRemoveRows(parent, row, row + count - 1);

    for (int i = 0; i < count; i++) {
        m_windows.removeAt(row);
    }

    endRemoveRows();
    return true;
}

#include "windowslist.moc"
