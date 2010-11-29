#include <libwnck/screen.h>
#include <libwnck/window.h>
#include <glib-2.0/glib.h>

#include <QDebug>

#include "windowslist.h"
#include "windowinfo.h"

#include "bamf-matcher.h"
#include "bamf-window.h"
#include "bamf-application.h"

WindowsList::WindowsList(QObject *parent) :
    QAbstractListModel(parent)
{
    QHash<int, QByteArray> roles;
    roles[0] = "item";
    setRoleNames(roles);

    BamfMatcher &matcher = BamfMatcher::get_default();

    BamfApplicationList *apps = matcher.applications();
    for (int i = 0; i < apps->size(); i++) {
        BamfApplication *app = apps->at(i);

        qDebug() << "+" << app->name() << " " << app->view_type();

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
                qDebug() << "!!!!!!!!!!! Had to force update";
                wnck_screen_force_update(wnck_screen_get_default());
                wnck_win = wnck_window_get(xid);
            }
            if (wnck_win == 0) continue;

            WnckWindowType type = wnck_window_get_window_type(wnck_win);
            if (type != WNCK_WINDOW_NORMAL &&
                type != WNCK_WINDOW_DIALOG &&
                type != WNCK_WINDOW_UTILITY) {
                continue;
            }

            qDebug().nospace() << "\t\t" << win->name() << " (" << xid << ")";

            WindowInfo *info = new WindowInfo(xid);
            m_windows.append(info);
        }
    }
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

    WindowInfo *info = m_windows.value(index.row(), 0);
    if (info == 0) return QVariant();
    else return QVariant::fromValue(info);
}
