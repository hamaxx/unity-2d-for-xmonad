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

// libindicator
#include <libindicator/indicator-desktop-shortcuts.h>

#include "launcheritem.h"

// libunity-2d
#include <gscopedpointer.h>

// Qt
#include <QObject>
#include <QUrl>
#include <QMetaType>
#include <QString>
#include <QTimer>
#include <QHash>
#include <QPointer>

#include "bamf-application.h"

struct SnStartupSequence;
extern "C" {
void sn_startup_sequence_unref(struct SnStartupSequence*);
}

class DBusMenuImporter;
class QFileSystemWatcher;

typedef GObjectScopedPointer<GAppInfo> GAppInfoPointer;
typedef GObjectScopedPointer<GDesktopAppInfo> GDesktopAppInfoPointer;
typedef GScopedPointer<SnStartupSequence, sn_startup_sequence_unref> SnStartupSequencePointer;
class LauncherApplication : public LauncherItem
{
    Q_OBJECT
    friend class LauncherApplicationsListDBUS;

    Q_PROPERTY(bool sticky READ sticky WRITE setSticky NOTIFY stickyChanged)
    Q_PROPERTY(QString application_type READ application_type NOTIFY applicationTypeChanged)
    Q_PROPERTY(QString desktop_file READ desktop_file WRITE setDesktopFile NOTIFY desktopFileChanged)
    Q_PROPERTY(QString executable READ executable NOTIFY executableChanged)
    Q_PROPERTY(bool has_visible_window READ has_visible_window NOTIFY hasVisibleWindowChanged)
    Q_PROPERTY(float progress READ progress NOTIFY progressChanged)
    Q_PROPERTY(bool progressBarVisible READ progressBarVisible NOTIFY progressBarVisibleChanged)
    Q_PROPERTY(int counter READ counter NOTIFY counterChanged)
    Q_PROPERTY(QString emblem READ emblem NOTIFY emblemChanged)
    Q_PROPERTY(bool counterVisible READ counterVisible NOTIFY counterVisibleChanged)
    Q_PROPERTY(bool emblemVisible READ emblemVisible NOTIFY emblemVisibleChanged)

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
    QString executable() const;
    virtual bool launching() const;
    bool has_visible_window() const;
    float progress() const;
    int counter() const;
    QString emblem() const;
    bool counterVisible() const;
    bool progressBarVisible() const;
    bool emblemVisible() const;

    /* setters */
    void setDesktopFile(const QString& desktop_file);
    void setSticky(bool sticky);
    void setBamfApplication(BamfApplication *application);
    void setSnStartupSequence(SnStartupSequence* sequence);

    /* methods */
    Q_INVOKABLE virtual void activate();
    Q_INVOKABLE void close();
    Q_INVOKABLE void spread(bool showAllWorkspaces = false);
    Q_INVOKABLE void setIconGeometry(int x, int y, int width, int height, uint xid=0);
    Q_INVOKABLE virtual void launchNewInstance();

    Q_INVOKABLE virtual void createMenuActions();

    static void showWindow(WnckWindow* window);
    static void moveViewportToWindow(WnckWindow* window);
    void updateOverlaysState(QMap<QString, QVariant> properties);

Q_SIGNALS:
    void stickyChanged(bool);
    void applicationTypeChanged(QString);
    void desktopFileChanged(QString);
    void executableChanged(QString);
    void hasVisibleWindowChanged(bool);
    void progressBarVisibleChanged(bool);
    void counterVisibleChanged(bool);
    void emblemVisibleChanged(bool);
    void progressChanged(float);
    void counterChanged(int);
    void emblemChanged(QString);

    void closed();

    void windowAdded(uint xid);

private Q_SLOTS:
    void onBamfApplicationClosed(bool running);
    void onLaunchingTimeouted();
    void updateHasVisibleWindow();
    void updateWindowCount();

    bool launch();
    void show();

    /* Contextual menu callbacks */
    void onStaticShortcutTriggered();
    void onKeepTriggered();
    void onQuitTriggered();

    void onWindowAdded(BamfWindow*);

    void slotChildAdded(BamfView*);
    void slotChildRemoved(BamfView*);
    void onIndicatorMenuUpdated();

    void onDesktopFileChanged(const QString&);
    void checkDesktopFileReallyRemoved();
    void beginForceUrgent(int duration);
    void endForceUrgent();

private:
    QPointer<BamfApplication> m_application;
    QFileSystemWatcher *m_desktopFileWatcher;
    GAppInfoPointer m_appInfo;
    SnStartupSequencePointer m_snStartupSequence;
    bool m_sticky;
    QTimer m_launching_timer;
    bool m_has_visible_window;
    QHash<QString, DBusMenuImporter*> m_indicatorMenus;
    int m_indicatorMenusReady;
    float m_progress;
    bool m_progressBarVisible;
    int m_counter;
    bool m_counterVisible;
    QString m_emblem;
    bool m_emblemVisible;
    bool m_forceUrgent;

    void updateBamfApplicationDependentProperties();
    void monitorDesktopFile(const QString&);
    void fetchIndicatorMenus();
    void createStaticMenuActions();
    int windowCountOnCurrentWorkspace();
    template<typename T>
    bool updateOverlayState(QMap<QString, QVariant> properties,
                            QString propertyName, T* member);

    IndicatorDesktopShortcuts* m_staticShortcuts;
};

Q_DECLARE_METATYPE(LauncherApplication*)

#endif // LAUNCHERAPPLICATION_H

