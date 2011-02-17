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

#ifndef LAUNCHERAPPLICATION_H
#define LAUNCHERAPPLICATION_H

#include <gio/gdesktopappinfo.h>
#include <libwnck/libwnck.h>

#include "launcheritem.h"

#include <QObject>
#include <QUrl>
#include <QMetaType>
#include <QString>
#include <QTimer>
#include <QHash>

#include "bamf-application.h"

class DBusMenuImporter;
class QFileSystemWatcher;

class LauncherApplication : public LauncherItem
{
    Q_OBJECT

    Q_PROPERTY(bool sticky READ sticky WRITE setSticky NOTIFY stickyChanged)
    Q_PROPERTY(QString application_type READ application_type NOTIFY applicationTypeChanged)
    Q_PROPERTY(QString desktop_file READ desktop_file WRITE setDesktopFile NOTIFY desktopFileChanged)
    Q_PROPERTY(int priority READ priority WRITE setPriority NOTIFY priorityChanged)
    Q_PROPERTY(bool has_visible_window READ has_visible_window NOTIFY hasVisibleWindowChanged)
    Q_PROPERTY(float progress READ progress NOTIFY progressChanged)
    Q_PROPERTY(bool progressBarVisible READ progressBarVisible NOTIFY progressBarVisibleChanged)
    Q_PROPERTY(int counter READ counter NOTIFY counterChanged)
    Q_PROPERTY(bool counterVisible READ counterVisible NOTIFY counterVisibleChanged)

public:
    LauncherApplication();
    LauncherApplication(const LauncherApplication& other);
    ~LauncherApplication();

    /* getters */
    virtual bool active() const;
    virtual bool running() const;
    virtual int windowCount() const;
    virtual bool urgent() const;
    bool sticky() const;
    virtual QString name() const;
    virtual QString icon() const;
    QString application_type() const;
    QString desktop_file() const;
    int priority() const;
    virtual bool launching() const;
    bool has_visible_window() const;
    float progress() const;
    int counter() const;
    bool counterVisible() const;
    bool progressBarVisible() const;

    /* setters */
    void setDesktopFile(const QString& desktop_file);
    void setPriority(int priority);
    void setSticky(bool sticky);
    void setBamfApplication(BamfApplication *application);

    /* methods */
    Q_INVOKABLE virtual void activate();
    Q_INVOKABLE void close();
    Q_INVOKABLE void spread();
    Q_INVOKABLE void setIconGeometry(int x, int y, int width, int height, uint xid=0);

    Q_INVOKABLE virtual void createMenuActions();

    static void showWindow(WnckWindow* window);
    static void moveViewportToWindow(WnckWindow* window);
    void updateOverlaysState(QMap<QString, QVariant> properties);

signals:
    void stickyChanged(bool);
    void applicationTypeChanged(QString);
    void desktopFileChanged(QString);
    void priorityChanged(int);
    void hasVisibleWindowChanged(bool);
    void progressBarVisibleChanged(bool);
    void counterVisibleChanged(bool);
    void progressChanged(float);
    void counterChanged(int);

    void closed();

    void windowAdded(uint xid);

private slots:
    void onBamfApplicationClosed(bool running);
    void onLaunchingTimeouted();
    void updateHasVisibleWindow();
    void updateWindowCount();

    bool launch();
    void show();

    /* Contextual menu callbacks */
    void onKeepTriggered();
    void onQuitTriggered();

    void onWindowAdded(BamfWindow*);

    void slotChildAdded(BamfView*);
    void slotChildRemoved(BamfView*);
    void onIndicatorMenuUpdated();

    void onDesktopFileChanged(const QString&);
    void checkDesktopFileReallyRemoved();

private:
    BamfApplication *m_application;
    QFileSystemWatcher *m_desktopFileWatcher;
    GDesktopAppInfo *m_appInfo;
    bool m_sticky;
    int m_priority;
    QTimer m_launching_timer;
    bool m_has_visible_window;
    QHash<QString, DBusMenuImporter*> m_indicatorMenus;
    int m_indicatorMenusReady;
    float m_progress;
    bool m_progressBarVisible;
    int m_counter;
    bool m_counterVisible;

    void updateBamfApplicationDependentProperties();
    void monitorDesktopFile(const QString&);
    void fetchIndicatorMenus();
    void createStaticMenuActions();
    int windowCountOnCurrentWorkspace();
    template<typename T>
    bool updateOverlayState(QMap<QString, QVariant> properties,
                            QString propertyName, T* member);
};

Q_DECLARE_METATYPE(LauncherApplication*)

#endif // LAUNCHERAPPLICATION_H

