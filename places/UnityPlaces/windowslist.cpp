/* Required otherwise using wnck_set_client_type breaks linking with error:
   undefined reference to `wnck_set_client_type(WnckClientType)'
*/
extern "C" {
#include <libwnck/screen.h>
#include <libwnck/window.h>
}
#include <glib-2.0/glib.h>

#include <QDebug>

#include "windowslist.h"
#include "windowinfo.h"

#include "bamf-matcher.h"
#include "bamf-window.h"
#include "bamf-application.h"

WindowsList::WindowsList(QObject *parent) :
    QAbstractListModel(parent), m_applicationId(0), m_loaded(false)
{
    QHash<int, QByteArray> roles;
    roles[0] = "window";
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
    Q_UNUSED(role);

    if (!index.isValid())
        return QVariant();

    WindowInfo *info = m_windows.at(index.row());
    return QVariant::fromValue(info);
}

void WindowsList::load(unsigned long applicationId) {
    if (m_loaded && m_applicationId == applicationId) {
        return;
    }

    BamfMatcher &matcher = BamfMatcher::get_default();
    QList<BamfApplication*> applications;

    if (applicationId == 0) {
        /* List the windows of all the applications */
        BamfApplicationList *allapplications = matcher.applications();
        for (int i = 0; i < allapplications->size(); i++) {
             applications.append(allapplications->at(i));
        }
    } else {
        /* List the windows of the application that has for group leader the window
           with XID applicationId */
        applications.append(matcher.application_for_xid(applicationId));
    }

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

    m_applicationId = applicationId;
    m_loaded = true;

    Q_EMIT countChanged(m_windows.count());
}

void WindowsList::unload() {
    beginRemoveRows(QModelIndex(), 0, m_windows.count() - 1);
    qDeleteAll(m_windows);
    m_windows.clear();
    endRemoveRows();

    m_loaded = false;

    Q_EMIT countChanged(m_windows.count());
}
