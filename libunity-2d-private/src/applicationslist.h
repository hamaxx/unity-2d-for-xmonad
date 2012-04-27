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

#ifndef APPLICATIONSLIST_H
#define APPLICATIONSLIST_H

#include <QAbstractListModel>
#include <QList>
#include <QVariant>
#include <QString>
#include <QStringList>
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
class Application;
class BamfApplication;
class BamfView;

class ApplicationsList : public QAbstractListModel, protected AbstractX11EventFilter
{
    Q_OBJECT
    friend class ApplicationsListDBUS;
    friend class ApplicationsListManager;

public:
    ApplicationsList(QObject *parent = 0);
    ~ApplicationsList();

    QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;
    int rowCount(const QModelIndex & parent = QModelIndex()) const;

public Q_SLOTS:
    void move(int from, int to);
    void moveFinished(int from, int to);

Q_SIGNALS:
    void applicationBecameUrgent(int index);

protected:
    bool x11EventFilter(XEvent* xevent);

private:
    void load();
    void insertBamfApplication(BamfApplication* bamf_application);
    void insertSnStartupSequence(SnStartupSequence* sequence);
    void insertFavoriteApplication(const QString& desktop_file);
    void insertWebFavorite(const QUrl& url);

    void insertApplication(Application* application);
    void removeApplication(Application* application);
    void removeApplication(const QString& desktop_file);

    QString favoriteFromDesktopFilePath(const QString& desktop_file) const;

    void writeFavoritesToGConf();

    void remoteEntryUpdated(const QString& desktopFile, const QString& sender, const QString& applicationURI, const QMap<QString, QVariant>& properties);

    void doMove(int from, int to);

    /* List of Application displayed in the launcher. */
    QList<Application*> m_applications;
    /* Hash of desktop file names to Application used to reduce
       the algorithmical complexity of merging the list of running applications
       and the list of favorited applications into the list of applications
       displayed (m_applications).
    */
    QHash<QString, Application*> m_applicationForDesktopFile;
    /* Hash of application executables to Application used to reduce
       the algorithmical complexity of merging the list of launching applications
       and the list of running applications into the list of applications
       displayed (m_applications).
    */
    QHash<QString, Application*> m_applicationForExecutable;
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
};

QML_DECLARE_TYPE(ApplicationsList)

#endif // APPLICATIONSLIST_H
