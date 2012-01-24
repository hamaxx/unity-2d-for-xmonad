/*
 * Copyright (C) 2010-2011 Canonical, Ltd.
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

#ifndef LAUNCHERAPPLICATIONSLIST_H
#define LAUNCHERAPPLICATIONSLIST_H

#include <QAbstractListModel>
#include <QList>
#include <QVariant>
#include <QString>
#include <QUrl>
#include <QObject>
#include <QtDeclarative/qdeclarative.h>
#include <QMap>
#include <QDBusContext>

#include <unity2dapplication.h>

struct SnDisplay;
struct SnMonitorContext;
struct SnMonitorEvent;
struct SnStartupSequence;
class LauncherApplication;
class BamfApplication;
class BamfView;
class QConf;

class LauncherApplicationsList : public QAbstractListModel, protected AbstractX11EventFilter, protected QDBusContext
{
    Q_OBJECT
    friend class LauncherApplicationsListDBUS;

public:
    LauncherApplicationsList(QObject *parent = 0);
    ~LauncherApplicationsList();

    QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;
    int rowCount(const QModelIndex & parent = QModelIndex()) const;

    Q_INVOKABLE void insertFavoriteApplication(QString desktop_file);
    Q_INVOKABLE void insertWebFavorite(const QUrl& url);

public Q_SLOTS:
    void move(int from, int to);

Q_SIGNALS:
    void applicationBecameUrgent(int index);

protected:
    bool x11EventFilter(XEvent* xevent);

private:
    void load();
    void insertBamfApplication(BamfApplication* bamf_application);
    void insertSnStartupSequence(SnStartupSequence* sequence);

    void insertApplication(LauncherApplication* application);
    void removeApplication(LauncherApplication* application);

    QString favoriteFromDesktopFilePath(const QString& desktop_file) const;

    void writeFavoritesToGConf();

    /* List of LauncherApplication displayed in the launcher. */
    QList<LauncherApplication*> m_applications;
    /* Hash of desktop file names to LauncherApplication used to reduce
       the algorithmical complexity of merging the list of running applications
       and the list of favorited applications into the list of applications
       displayed (m_applications).
    */
    QHash<QString, LauncherApplication*> m_applicationForDesktopFile;
    /* Hash of application executables to LauncherApplication used to reduce
       the algorithmical complexity of merging the list of launching applications
       and the list of running applications into the list of applications
       displayed (m_applications).
    */
    QHash<QString, LauncherApplication*> m_applicationForExecutable;
    QConf* m_dconf_launcher;
    QStringList m_xdgApplicationDirs;

    /* Startup notification support */
    SnDisplay *m_snDisplay;
    SnMonitorContext *m_snContext;
    static void snEventHandler(SnMonitorEvent *event, void *user_data);
    void onSnMonitorEventReceived(SnMonitorEvent *event);

private Q_SLOTS:
    void onApplicationClosed();
    void onBamfViewOpened(BamfView* bamf_view);
    void onApplicationStickyChanged(bool sticky);
    void onApplicationLaunchingChanged(bool launching);
    void onApplicationUrgentChanged(bool urgent);
    void onApplicationUserVisibleChanged(bool user_visible);
    void onRemoteEntryUpdated(QString applicationURI,
                              QMap<QString, QVariant> properties);
};

QML_DECLARE_TYPE(LauncherApplicationsList)

#endif // LAUNCHERAPPLICATIONSLIST_H
