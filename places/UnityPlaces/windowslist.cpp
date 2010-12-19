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
    QList<WindowInfo*> newWins;
    BamfMatcher &matcher = BamfMatcher::get_default();

    QList<BamfApplication*> apps;
    if (applicationId) {
        apps.append(matcher.application_for_xid(applicationId));
    } else {
        BamfApplicationList *allapps = matcher.applications();
        for (int i = 0; i < allapps->size(); i++) {
             apps.append(allapps->at(i));
        }
    }

    foreach (BamfApplication* app, apps) {
       //qDebug() << "+" << app->name() << " " << app->view_type();

        BamfWindowList *wins = app->windows();
        for (int k = 0; k < wins->size(); k++) {
            BamfWindow* win = wins->at(k);
            if (win == 0) {
                qDebug() << "Window belonging to " << app->name()
                         << "is in list but null";
                continue;
            }

            Window xid = win->xid();

            WnckWindow *wnck_win = wnck_window_get(xid);
            if (wnck_win == 0) {
                wnck_screen_force_update(wnck_screen_get_default());
                wnck_win = wnck_window_get(xid);
            }
            if (wnck_win == 0) continue;

            if (wnck_window_is_skip_tasklist(wnck_win)) {
                continue;
            }

            WnckWindowType type = wnck_window_get_window_type(wnck_win);
            if (type != WNCK_WINDOW_NORMAL &&
                type != WNCK_WINDOW_DIALOG &&
                type != WNCK_WINDOW_UTILITY) {
                continue;
            }

            //qDebug().nospace() << "\t\t" << win->name() << " (" << xid << ")";

            WindowInfo *info = new WindowInfo(xid);
            newWins.append(info);
        }
    }

    qDebug() << "Matched" << newWins.count() << "Windows in" << apps.count() << "Applications";

    if (m_windows.count() > 0) {
        beginRemoveRows(QModelIndex(), 0, m_windows.count() - 1);
        qDeleteAll(m_windows);
        m_windows.clear();
        endRemoveRows();
    }

    if (newWins.count() > 0) {
        beginInsertRows(QModelIndex(), 0, newWins.count() - 1);
        m_windows.append(newWins);
        endInsertRows();
    }

    m_applicationId = applicationId;
    m_loaded = true;

    emit applicationIdChanged(m_applicationId);
    emit countChanged(m_windows.count());
    emit loadedChanged(m_loaded);
}

void WindowsList::unload() {
    beginRemoveRows(QModelIndex(), 0, m_windows.count() - 1);
    qDeleteAll(m_windows);
    m_windows.clear();
    endRemoveRows();

    m_loaded = false;

    emit loadedChanged(m_loaded);
    emit countChanged(m_windows.count());
}
