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

    QList<WindowInfo*> newWindows;
    BamfMatcher &matcher = BamfMatcher::get_default();

    QList<BamfApplication*> applications;
    if (applicationId) {
        applications.append(matcher.application_for_xid(applicationId));
    } else {
        BamfApplicationList *allapplications = matcher.applications();
        for (int i = 0; i < allapplications->size(); i++) {
             applications.append(allapplications->at(i));
        }
    }

    foreach (BamfApplication* application, applications) {
       //qDebug() << "+" << application->name() << " " << application->view_type();

        BamfWindowList *windows = application->windows();
        for (int k = 0; k < windows->size(); k++) {
            BamfWindow* window = windows->at(k);
            if (window == 0) {
                qDebug() << "Window belonging to " << application->name()
                         << "is in list but null";
                continue;
            }

            Window xid = window->xid();

            WnckWindow *wnck_window = wnck_window_get(xid);
            if (wnck_window == 0) {
                wnck_screen_force_update(wnck_screen_get_default());
                wnck_window = wnck_window_get(xid);
            }
            if (wnck_window == 0) continue;

            if (wnck_window_is_skip_tasklist(wnck_window)) {
                continue;
            }

            WnckWindowType type = wnck_window_get_window_type(wnck_window);
            if (type != WNCK_WINDOW_NORMAL &&
                type != WNCK_WINDOW_DIALOG &&
                type != WNCK_WINDOW_UTILITY) {
                continue;
            }

            //qDebug().nospace() << "\t\t" << window->name() << " (" << xid << ")";

            WindowInfo *info = new WindowInfo(xid);
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

    Q_EMIT applicationIdChanged(m_applicationId);
    Q_EMIT countChanged(m_windows.count());
    Q_EMIT loadedChanged(m_loaded);
}

void WindowsList::unload() {
    beginRemoveRows(QModelIndex(), 0, m_windows.count() - 1);
    qDeleteAll(m_windows);
    m_windows.clear();
    endRemoveRows();

    m_loaded = false;

    Q_EMIT countChanged(m_windows.count());
    Q_EMIT loadedChanged(m_loaded);
}
