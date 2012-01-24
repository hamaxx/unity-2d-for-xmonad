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

/* Note regarding the use of wnck: it is critically important that the client
   type be set to pager because wnck will pass that type over to the window
   manager through XEvents.
   Window managers tend to respect orders from pagers to the letter by for
   example bypassing focus stealing prevention. Compiz does exactly that in
   src/event.c:handleEvent(…) in the ClientMessage case (line 1702). Metacity
   has a similar policy in src/core/window.c:window_activate(…) (line 2951).

   The client type has already been set in Unity2dPlugin::initializeEngine(…),
   and the corresponding shared library (libunity-2d-private-qml.so) is loaded
   via QML’s import statement (`import Unity2d 1.0`), so there is no need to
   set it again here.
*/

#include "launcherapplication.h"
#include "launchermenu.h"
#include "launcherutility.h"
#include "bamf-matcher.h"
#include "bamf-indicator.h"

#include "dbusmenuimporter.h"

#include <X11/X.h>

// libunity-2d
#include <unity2dtr.h>
#include <debug_p.h>

// Qt
#include <Qt>
#include <QDebug>
#include <QAction>
#include <QDBusInterface>
#include <QDBusReply>
#include <QDBusServiceWatcher>
#include <QFile>
#include <QFileSystemWatcher>
#include <QScopedPointer>
#include <QX11Info>

extern "C" {
#include <gdk/gdk.h>
#include <gdk/gdkx.h>
#include <libsn/sn.h>
}

const char* SHORTCUT_NICK_PROPERTY = "nick";

LauncherApplication::LauncherApplication()
    : m_application(NULL)
    , m_desktopFileWatcher(NULL)
    , m_sticky(false)
    , m_has_visible_window(false)
    , m_progress(0), m_progressBarVisible(false)
    , m_counter(0), m_counterVisible(false)
    , m_emblem(QString()), m_emblemVisible(false)
    , m_forceUrgent(false)
    , m_dynamicQuicklistServiceWatcher(NULL)
{
    m_launching_timer.setSingleShot(true);
    m_launching_timer.setInterval(8000);
    QObject::connect(&m_launching_timer, SIGNAL(timeout()), this, SLOT(onLaunchingTimeouted()));
}

LauncherApplication::LauncherApplication(const LauncherApplication& other)
{
    /* FIXME: a number of members are not copied over */
    QObject::connect(&m_launching_timer, SIGNAL(timeout()), this, SLOT(onLaunchingTimeouted()));
    if (other.m_application != NULL) {
        setBamfApplication(other.m_application);
    }
}

LauncherApplication::~LauncherApplication()
{
}

bool
LauncherApplication::active() const
{
    if (m_application != NULL) {
        return m_application->active();
    }

    return false;
}

bool
LauncherApplication::running() const
{
    if (m_application != NULL) {
        return m_application->running();
    }

    return false;
}

int
LauncherApplication::windowCount() const
{
    if (m_application == NULL) {
        return 0;
    }

    QScopedPointer<BamfWindowList> windows(m_application->windows());
    return windows->size();
}

bool
LauncherApplication::urgent() const
{
    if (m_forceUrgent) {
        return true;
    }

    if (m_application != NULL) {
        return m_application->urgent();
    }

    return false;
}

void
LauncherApplication::beginForceUrgent(int duration)
{
    bool wasUrgent = urgent();
    m_forceUrgent = true;
    if (wasUrgent != urgent()) {
        Q_EMIT urgentChanged(urgent());
    }
    QTimer::singleShot(duration, this, SLOT(endForceUrgent()));
}

void
LauncherApplication::endForceUrgent()
{
    bool wasUrgent = urgent();
    m_forceUrgent = false;
    if (wasUrgent != urgent()) {
        Q_EMIT urgentChanged(urgent());
    }
}

bool
LauncherApplication::sticky() const
{
    return m_sticky;
}

QString
LauncherApplication::name() const
{
    if (sticky() && (m_appInfo != NULL)) {
        return QString::fromUtf8(g_app_info_get_name(m_appInfo.data()));
    }

    if (m_application != NULL) {
        return m_application->name();
    }

    if (m_appInfo != NULL) {
        return QString::fromUtf8(g_app_info_get_name(m_appInfo.data()));
    }

    if (m_snStartupSequence != NULL) {
        return QString::fromUtf8(sn_startup_sequence_get_name(m_snStartupSequence.data()));
    }

    return QString("");
}

QString
LauncherApplication::icon() const
{
    if (sticky() && (m_appInfo != NULL)) {
        GCharPointer ptr(g_icon_to_string(g_app_info_get_icon(m_appInfo.data())));
        return QString::fromUtf8(ptr.data());
    }

    if (m_application != NULL) {
        return m_application->icon();
    }

    if (m_appInfo != NULL) {
        GCharPointer ptr(g_icon_to_string(g_app_info_get_icon(m_appInfo.data())));
        return QString::fromUtf8(ptr.data());
    }

    if (m_snStartupSequence != NULL) {
        return QString::fromUtf8(sn_startup_sequence_get_icon_name(m_snStartupSequence.data()));
    }

    return QString("");
}

QString
LauncherApplication::application_type() const
{
    if (m_application != NULL) {
        return m_application->application_type();
    }

    return QString("");
}

QString
LauncherApplication::desktop_file() const
{
    if (m_application != NULL) {
        return m_application->desktop_file();
    }

    if (m_appInfo != NULL) {
        return QString::fromUtf8(g_desktop_app_info_get_filename((GDesktopAppInfo*)m_appInfo.data()));
    }

    return QString("");
}

QString
LauncherApplication::executable() const
{
    if (m_appInfo != NULL) {
        return QString::fromUtf8(g_app_info_get_executable(m_appInfo.data()));
    }

    if (m_snStartupSequence != NULL) {
        return QString::fromUtf8(sn_startup_sequence_get_binary_name(m_snStartupSequence.data()));
    }

    return QString("");
}

void
LauncherApplication::setSticky(bool sticky)
{
    if (sticky == m_sticky) {
        return;
    }

    m_sticky = sticky;
    stickyChanged(sticky);
}

void
LauncherApplication::setDesktopFile(const QString& desktop_file)
{
    QString oldDesktopFile = this->desktop_file();

    QByteArray byte_array = desktop_file.toUtf8();
    gchar *file = byte_array.data();

    if(desktop_file.startsWith("/")) {
        /* It looks like a full path to a desktop file */
        m_appInfo.reset((GAppInfo*)g_desktop_app_info_new_from_filename(file));
    } else {
        /* It might just be a desktop file name; let GIO look for the actual
           desktop file for us */
        /* The docs for g_desktop_app_info_new() says it respects "-" to "/"
           substitution as per XDG Menu Spec, but it only seems to work for
           exactly 1 substitution where as Wine programs often require many.
           Bottom line: We must do some manual trial and error to find desktop
           files in deeply nested directories.

           Same workaround is implemented in Unity: plugins/unityshell/src/PlacesView.cpp:731
           References:
           https://bugzilla.gnome.org/show_bug.cgi?id=654566
           https://bugs.launchpad.net/unity-2d/+bug/794471
        */
        int slash_index;
        do {
            m_appInfo.reset((GAppInfo*)g_desktop_app_info_new(file));
            slash_index = byte_array.indexOf("-");
            if (slash_index == -1) {
                break;
            }
            byte_array.replace(slash_index, 1, "/");
            file = byte_array.data();
        } while (m_appInfo.isNull());
    }

    /* setDesktopFile(…) may be called with the same desktop file, when e.g. the
       contents of the file have changed. Some properties may have changed. */
    QString newDesktopFile = this->desktop_file();
    if (newDesktopFile != oldDesktopFile) {
        Q_EMIT desktopFileChanged(newDesktopFile);
    }
    /* Emit the Changed signal on all properties that can depend on m_appInfo
       m_application's properties take precedence over m_appInfo's
    */
    if (m_appInfo != NULL) {
        if (m_application == NULL) {
            Q_EMIT nameChanged(name());
            Q_EMIT iconChanged(icon());
        }
        Q_EMIT executableChanged(executable());
    }

    /* Update the list of static shortcuts
       (quicklist entries defined in the desktop file). */
    m_staticShortcuts.reset(indicator_desktop_shortcuts_new(newDesktopFile.toUtf8().constData(), "Unity"));

    monitorDesktopFile(newDesktopFile);
}

void
LauncherApplication::monitorDesktopFile(const QString& path)
{
    /* Monitor the desktop file for live changes */
    if (m_desktopFileWatcher == NULL) {
        /* FIXME: deleting a QFileSystemWatcher can be quite slow (sometimes
           around 100ms on a powerful computer) and can provoke visual glitches
           where the user interface is blocked for a short moment.
         */
        m_desktopFileWatcher = new QFileSystemWatcher(this);
        connect(m_desktopFileWatcher, SIGNAL(fileChanged(const QString&)),
                SLOT(onDesktopFileChanged(const QString&)));
    }

    /* If the file is already being monitored, we shouldn’t need to do anything.
       However it seems that in some cases, a change to the file will stop
       emiting further fileChanged signals, despite the file still being in the
       list of monitored files. This is the case when the desktop file is being
       edited in gedit for example. This may be a bug in QT itself.
       To work around this issue, remove the path and add it again. */
    if (m_desktopFileWatcher->files().contains(path)) {
        m_desktopFileWatcher->removePath(path);
    }

    if (!path.isEmpty()) {
        m_desktopFileWatcher->addPath(path);
    }
}

void
LauncherApplication::onDesktopFileChanged(const QString& path)
{
    if (m_desktopFileWatcher->files().contains(path) || QFile::exists(path)) {
        /* The contents of the file have changed. */
        setDesktopFile(path);
    } else {
        /* The desktop file has been deleted.
           This can happen in a number of cases:
            - the package it belongs to has been uninstalled
            - the package it belongs to has been upgraded, in which case it is
              likely that the desktop file has been removed and a new version of
              it has been installed in place of the old version
            - the file has been written to using an editor that first saves to a
              temporary file and then moves this temporary file to the
              destination file, which effectively results in the file being
              temporarily deleted (vi for example does that, whereas gedit
              doesn’t)
           In the first case, we want to remove the application from the
           launcher. In the last two cases, we need to consider that the desktop
           file’s contents have changed. At this point there is no way to be
           sure that the file has been permanently removed, so we want to give
           the application a grace period before checking for real deletion. */
        QTimer::singleShot(1000, this, SLOT(checkDesktopFileReallyRemoved()));
    }
}

void
LauncherApplication::checkDesktopFileReallyRemoved()
{
    QString path = desktop_file();
    if (QFile::exists(path)) {
        /* The desktop file hasn’t really been removed, it was only temporarily
           deleted. */
        setDesktopFile(path);
    } else {
        /* The desktop file has really been removed. */
        setSticky(false);
    }
}

void
LauncherApplication::setBamfApplication(BamfApplication *application)
{
    if (application == NULL) {
        return;
    }

    m_application = application;
    if (!sticky()) {
        setDesktopFile(application->desktop_file());
    }

    QObject::connect(application, SIGNAL(ActiveChanged(bool)), this, SIGNAL(activeChanged(bool)));

    QObject::connect(application, SIGNAL(RunningChanged(bool)), this, SLOT(updateCounterVisible()));
    /* FIXME: a bug somewhere makes connecting to the Closed() signal not work even though
              the emit Closed() in bamf-view.cpp is reached. */
    /* Connect first the onBamfApplicationClosed slot, then the runningChanged
       signal, to avoid a race condition when an application is closed.
       See https://launchpad.net/bugs/634057 */
    QObject::connect(application, SIGNAL(RunningChanged(bool)), this, SLOT(onBamfApplicationClosed(bool)));
    QObject::connect(application, SIGNAL(RunningChanged(bool)), this, SIGNAL(runningChanged(bool)));
    QObject::connect(application, SIGNAL(UrgentChanged(bool)), this, SIGNAL(urgentChanged(bool)));
    QObject::connect(application, SIGNAL(WindowAdded(BamfWindow*)), this, SLOT(updateHasVisibleWindow()));
    QObject::connect(application, SIGNAL(WindowRemoved(BamfWindow*)), this, SLOT(updateHasVisibleWindow()));
    QObject::connect(application, SIGNAL(WindowAdded(BamfWindow*)), this, SLOT(updateWindowCount()));
    QObject::connect(application, SIGNAL(WindowRemoved(BamfWindow*)), this, SLOT(updateWindowCount()));
    connect(application, SIGNAL(ChildAdded(BamfView*)), SLOT(slotChildAdded(BamfView*)));
    connect(application, SIGNAL(ChildRemoved(BamfView*)), SLOT(slotChildRemoved(BamfView*)));

    connect(application, SIGNAL(WindowAdded(BamfWindow*)), SLOT(onWindowAdded(BamfWindow*)));

    updateBamfApplicationDependentProperties();
    updateCounterVisible();
}

void
LauncherApplication::updateBamfApplicationDependentProperties()
{
    activeChanged(active());
    runningChanged(running());
    urgentChanged(urgent());
    nameChanged(name());
    iconChanged(icon());
    applicationTypeChanged(application_type());
    desktopFileChanged(desktop_file());
    m_launching_timer.stop();
    launchingChanged(launching());
    updateHasVisibleWindow();
    updateWindowCount();
    fetchIndicatorMenus();
}

void
LauncherApplication::onBamfApplicationClosed(bool running)
{
    if(running)
       return;

    m_application->disconnect(this);
    m_application = NULL;
    updateBamfApplicationDependentProperties();
    closed();
}

void
LauncherApplication::setSnStartupSequence(SnStartupSequence* sequence)
{
    if (sequence != NULL) {
        if (!sn_startup_sequence_get_completed(sequence)) {
            /* 'launching' property becomes true for a few seconds */
            m_launching_timer.start();
        } else {
            m_launching_timer.stop();
        }
        sn_startup_sequence_ref(sequence);
    }

    m_snStartupSequence.reset(sequence);

    nameChanged(name());
    iconChanged(icon());
    executableChanged(executable());
    launchingChanged(launching());
}

void
LauncherApplication::setIconGeometry(int x, int y, int width, int height, uint xid)
{
    if (m_application == NULL) {
        return;
    }

    QScopedPointer<BamfUintList> xids;
    if (xid == 0) {
        xids.reset(m_application->xids());
    } else {
        QList<uint> list;
        list.append(xid);
        xids.reset(new BamfUintList(list));
    }
    int size = xids->size();
    if (size < 1) {
        return;
    }

    WnckScreen* screen = wnck_screen_get_default();
    wnck_screen_force_update(screen);

    for (int i = 0; i < size; ++i) {
        WnckWindow* window = wnck_window_get(xids->at(i));
        wnck_window_set_icon_geometry(window, x, y, width, height);
    }
}

void
LauncherApplication::onWindowAdded(BamfWindow* window)
{
    if (window != NULL) {
        windowAdded(window->xid());
    }
}

bool
LauncherApplication::launching() const
{
    return m_launching_timer.isActive();
}

void
LauncherApplication::updateHasVisibleWindow()
{
    bool prev = m_has_visible_window;

    if (m_application != NULL) {
        m_has_visible_window = QScopedPointer<BamfUintList>(m_application->xids())->size() > 0;
    } else {
        m_has_visible_window = false;
    }

    if (m_has_visible_window != prev) {
        hasVisibleWindowChanged(m_has_visible_window);
    }
}

void
LauncherApplication::updateWindowCount()
{
    Q_EMIT windowCountChanged(windowCount());
}

void
LauncherApplication::updateCounterVisible()
{
    bool counterVisible = running() && m_counter > 0;

    if (m_counterVisible != counterVisible) {
        m_counterVisible = counterVisible;
        Q_EMIT counterVisibleChanged(m_counterVisible);
    }
}

bool
LauncherApplication::has_visible_window() const
{
    return m_has_visible_window;
}

float
LauncherApplication::progress() const
{
    return m_progress;
}

int
LauncherApplication::counter() const
{
    return m_counter;
}

QString
LauncherApplication::emblem() const
{
    return m_emblem;
}

bool
LauncherApplication::progressBarVisible() const
{
    return m_progressBarVisible;
}

bool
LauncherApplication::counterVisible() const
{
    return m_counterVisible;
}

bool
LauncherApplication::emblemVisible() const
{
    return m_emblemVisible;
}

/* Returns the number of window for this application that reside on the
   current workspace */
int
LauncherApplication::windowCountOnCurrentWorkspace()
{
    int windowCount = 0;
    WnckWorkspace *current = wnck_screen_get_active_workspace(wnck_screen_get_default());

    for (int i = 0; i < m_application->windows()->size(); i++) {
        BamfWindow *window = m_application->windows()->at(i);
        if (window == NULL) {
            continue;
        }

        /* When geting the wnck window, it's possible we get a NULL
           return value because wnck hasn't updated its internal list yet,
           so we need to force it once to be sure */
        WnckWindow *wnck_window = wnck_window_get(window->xid());
        if (wnck_window == NULL) {
            wnck_screen_force_update(wnck_screen_get_default());
            wnck_window = wnck_window_get(window->xid());
            if (wnck_window == NULL) {
                continue;
            }
        }

        WnckWorkspace *workspace = wnck_window_get_workspace(wnck_window);
        if (workspace == current) {
            windowCount++;
        }
    }
    return windowCount;
}

void
LauncherApplication::activate()
{
    if (urgent()) {
        show();
    } else if (active()) {
        if (windowCountOnCurrentWorkspace() > 0 && windowCount() > 1) {
            spread(windowCount() > windowCountOnCurrentWorkspace());
        }
    } else if (running() && has_visible_window()) {
        show();
    } else {
        launch();
    }
}

void
LauncherApplication::launchNewInstance()
{
    launch();
}

bool
LauncherApplication::launch()
{
    if (m_appInfo == NULL) {
        return false;
    }

    GError* error = NULL;

    GdkWindow* root;
    guint32 timestamp;

    GdkDisplay* display = gdk_display_get_default();
    GObjectScopedPointer<GdkAppLaunchContext> context(gdk_display_get_app_launch_context(display));

    /* We need to take the timestamp from the X server for the
       window manager to work properly
       https://bugs.launchpad.net/unity-2d/+bug/735205 */
    /* FIXME: ultimately we should forward the timestamps from
       events that triggered the launch */
    root = gdk_x11_window_lookup_for_display(display, GDK_ROOT_WINDOW());
    timestamp = gdk_x11_get_server_time(root);

    gdk_app_launch_context_set_timestamp(context.data(), timestamp);

    g_app_info_launch(m_appInfo.data(), NULL, (GAppLaunchContext*)context.data(), &error);

    if (error != NULL) {
        UQ_WARNING << "Failed to launch application:" << error->message;
        g_error_free(error);
        return false;
    }

    /* 'launching' property becomes true for a few seconds and becomes
       false as soon as the application is launched */
    m_launching_timer.start();
    launchingChanged(true);

    return true;
}

void
LauncherApplication::onLaunchingTimeouted()
{
    launchingChanged(false);
}

void
LauncherApplication::close()
{
    if (m_application == NULL) {
        return;
    }

    QScopedPointer<BamfUintList> xids(m_application->xids());
    int size = xids->size();
    if (size < 1) {
        return;
    }

    WnckScreen* screen = wnck_screen_get_default();
    wnck_screen_force_update(screen);

    for (int i = 0; i < size; ++i) {
        WnckWindow* window = wnck_window_get(xids->at(i));
        wnck_window_close(window, CurrentTime);
    }
}

void
LauncherApplication::show()
{
    if (m_application == NULL) {
        return;
    }

    QScopedPointer<BamfWindowList> windows(m_application->windows());
    int size = windows->size();
    if (size < 1) {
        return;
    }

    /* Pick the most important window.
       The primary criterion to determine the most important window is urgency.
       The secondary criterion is the last_active timestamp (the last time the
       window was activated). */
    BamfWindow* important = windows->at(0);
    for (int i = 0; i < size; ++i) {
        BamfWindow* current = windows->at(i);
        if (current->urgent() && !important->urgent()) {
            important = current;
        } else if (current->urgent() || !important->urgent()) {
            if (current->last_active() > important->last_active()) {
                important = current;
            }
        }
    }

    WnckScreen* screen = wnck_screen_get_default();
    wnck_screen_force_update(screen);

    WnckWindow* window = wnck_window_get(important->xid());
    LauncherUtility::showWindow(window);
}

void
LauncherApplication::spread(bool showAllWorkspaces)
{
    QDBusInterface compiz("org.freedesktop.compiz",
                          "/org/freedesktop/compiz/scale/screen0/initiate_all_key",
                          "org.freedesktop.compiz");

    if (compiz.isValid()) {
        Qt::HANDLE root = QX11Info::appRootWindow();
        BamfUintList* xids = m_application->xids();
        QStringList fragments;
        for (int i = 0; i < xids->size(); i++) {
            uint xid = xids->at(i);
            fragments.append("xid=" + QString::number(xid));
        }

        compiz.asyncCall("activate", "root", static_cast<int>(root), "match", fragments.join(" | "));
    } else {
        QDBusInterface spread("com.canonical.Unity2d.Spread", "/Spread",
                              "com.canonical.Unity2d.Spread");
        QDBusReply<bool> isShown = spread.call("IsShown");
        if (isShown.isValid()) {
            if (isShown.value() == true) {
                spread.asyncCall("FilterByApplication", m_application->desktop_file());
            } else {
                if (showAllWorkspaces) {
                    spread.asyncCall("ShowAllWorkspaces", m_application->desktop_file());
                } else {
                    spread.asyncCall("ShowCurrentWorkspace", m_application->desktop_file());
                }
            }
        } else {
            UQ_WARNING << "Failed to get property IsShown on com.canonical.Unity2d.Spread";
        }
    }
}

void
LauncherApplication::slotChildAdded(BamfView* child)
{
    BamfIndicator* indicator = qobject_cast<BamfIndicator*>(child);
    if (indicator != NULL) {
        QString path = indicator->dbus_menu_path();
        if (!m_indicatorMenus.contains(path)) {
            DBusMenuImporter* importer = new DBusMenuImporter(indicator->address(), path, this);
            connect(importer, SIGNAL(menuUpdated()), SLOT(onIndicatorMenuUpdated()));
            m_indicatorMenus[path] = importer;
        }
    }
}

void
LauncherApplication::slotChildRemoved(BamfView* child)
{
    BamfIndicator* indicator = qobject_cast<BamfIndicator*>(child);
    if (indicator != NULL) {
        QString path = indicator->dbus_menu_path();
        if (m_indicatorMenus.contains(path)) {
            m_indicatorMenus.take(path)->deleteLater();
        }
    }
}

void
LauncherApplication::fetchIndicatorMenus()
{
    Q_FOREACH(QString path, m_indicatorMenus.keys()) {
        m_indicatorMenus.take(path)->deleteLater();
    }

    if (m_application != NULL) {
        QScopedPointer<BamfViewList> children(m_application->children());
        for (int i = 0; i < children->size(); ++i) {
            slotChildAdded(children->at(i));
        }
    }
}

void
LauncherApplication::createMenuActions()
{
    if (m_application != NULL && !m_indicatorMenus.isEmpty()) {
        /* Request indicator menus to be updated: this is asynchronous
           and the corresponding actions are added to the menu in
           SLOT(onIndicatorMenuUpdated()).
           Static menu actions are appended after all indicator menus
           have been updated.*/
        m_indicatorMenusReady = 0;
        Q_FOREACH(DBusMenuImporter* importer, m_indicatorMenus) {
            importer->updateMenu();
        }
    } else {
        createDynamicMenuActions();
        createStaticMenuActions();
    }
}

void
LauncherApplication::createDynamicMenuActions()
{
    if (!m_dynamicQuicklistImporter.isNull()) {
        /* FIXME: the menu is only partially updated while visible: stale
           actions will correctly be removed from the menu, but new actions
           will not be added until the menu is closed and opened again.
           This is an acceptable limitation for now. */
        QList<QAction*> actions = m_dynamicQuicklistImporter->menu()->actions();
        Q_FOREACH(QAction* action, actions) {
            if (action->isSeparator()) {
                m_menu->insertSeparatorBeforeTitle();
            } else {
                connect(action, SIGNAL(triggered()), m_menu, SLOT(hide()));
                m_menu->insertActionBeforeTitle(action);
            }
        }
    }
}

void
LauncherApplication::createStaticMenuActions()
{
    /* Custom menu actions from the desktop file. */
    if (!m_staticShortcuts.isNull()) {
        const gchar** nicks = indicator_desktop_shortcuts_get_nicks(m_staticShortcuts.data());
        if (nicks) {
            int i = 0;
            while (((gpointer*) nicks)[i]) {
                const gchar* nick = nicks[i];
                QAction* action = new QAction(m_menu);
                action->setText(QString::fromUtf8(indicator_desktop_shortcuts_nick_get_name(m_staticShortcuts.data(), nick)));
                action->setProperty(SHORTCUT_NICK_PROPERTY, QVariant(nick));
                m_menu->insertActionBeforeTitle(action);
                connect(action, SIGNAL(triggered()), SLOT(onStaticShortcutTriggered()));
                ++i;
            }
        }
    }
    m_menu->insertSeparatorBeforeTitle();

    QList<QAction*> actions;
    bool is_running = running();

    /* Only applications with a corresponding desktop file can be kept in the launcher */
    if (QFile::exists(desktop_file())) {
        QAction* keep = new QAction(m_menu);
        keep->setCheckable(is_running);
        keep->setChecked(sticky());
        keep->setText(is_running ? u2dTr("Keep in launcher") : u2dTr("Remove from launcher"));
        actions.append(keep);
        QObject::connect(keep, SIGNAL(triggered()), this, SLOT(onKeepTriggered()));
    }

    if (is_running) {
        QAction* quit = new QAction(m_menu);
        quit->setText(u2dTr("Quit"));
        actions.append(quit);
        QObject::connect(quit, SIGNAL(triggered()), this, SLOT(onQuitTriggered()));
    }

    /* Filter out duplicate actions. This typically happens with indicator
       menus that contain a "Quit" action: we don’t want two "Quit" actions in
       our menu. */
    Q_FOREACH(QAction* pending, actions) {
        bool duplicate = false;
        Q_FOREACH(QAction* action, m_menu->actions()) {
            /* The filtering is done on the text of the action. This will
               obviously break if for example only one of the two actions is
               localized ("Quit" != "Quitter"), but we don’t have a better way
               to identify duplicate actions. */
            if (pending->text() == action->text()) {
                duplicate = true;
                break;
            }
        }

        if (!duplicate) {
            m_menu->addAction(pending);
        } else {
            delete pending;
        }
    } 
}

void
LauncherApplication::onIndicatorMenuUpdated()
{
    if (!m_menu->isVisible()) {
        return;
    }

    DBusMenuImporter* importer = static_cast<DBusMenuImporter*>(sender());
    QList<QAction*> actions = importer->menu()->actions();
    Q_FOREACH(QAction* action, actions) {
        if (action->isSeparator()) {
            m_menu->insertSeparatorBeforeTitle();
        } else {
            connect(action, SIGNAL(triggered()), m_menu, SLOT(hide()));
            m_menu->insertActionBeforeTitle(action);
        }
    }

    if (++m_indicatorMenusReady == m_indicatorMenus.size()) {
        /* All indicator menus have been updated. */
        createDynamicMenuActions();
        createStaticMenuActions();
    }
}

void
LauncherApplication::onStaticShortcutTriggered()
{
    QAction* action = static_cast<QAction*>(sender());
    QString nick = action->property(SHORTCUT_NICK_PROPERTY).toString();
    m_menu->hide();
    indicator_desktop_shortcuts_nick_exec(m_staticShortcuts.data(), nick.toUtf8().constData());
}

void
LauncherApplication::onKeepTriggered()
{
    QAction* keep = static_cast<QAction*>(sender());
    bool sticky = keep->isChecked();
    m_menu->hide();
    setSticky(sticky);
}

void
LauncherApplication::onQuitTriggered()
{
    m_menu->hide();
    close();
}

template<typename T>
bool LauncherApplication::updateOverlayState(QMap<QString, QVariant> properties,
                                             QString propertyName, T* member)
{
    if (properties.contains(propertyName)) {
        T value = properties.value(propertyName).value<T>();
        if (value != *member) {
            *member = value;
            return true;
        }
    }
    return false;
}

void
LauncherApplication::updateOverlaysState(const QString& sender, QMap<QString, QVariant> properties)
{
    if (updateOverlayState(properties, "progress", &m_progress)) {
        Q_EMIT progressChanged(m_progress);
    }
    if (updateOverlayState(properties, "progress-visible", &m_progressBarVisible)) {
        Q_EMIT progressBarVisibleChanged(m_progressBarVisible);
    }
    if (updateOverlayState(properties, "count", &m_counter)) {
        Q_EMIT counterChanged(m_counter);
    }
    if (updateOverlayState(properties, "count-visible", &m_counterVisible)) {
        Q_EMIT counterVisibleChanged(m_counterVisible);
    }
    if (updateOverlayState(properties, "emblem", &m_emblem)) {
        Q_EMIT emblemChanged(m_emblem);
    }
    if (updateOverlayState(properties, "emblem-visible", &m_emblemVisible)) {
        Q_EMIT emblemVisibleChanged(m_emblemVisible);
    }
    if (updateOverlayState(properties, "quicklist", &m_dynamicQuicklistPath)) {
        setDynamicQuicklistImporter(sender);
    }
}

void
LauncherApplication::setDynamicQuicklistImporter(const QString& service)
{
    if (m_dynamicQuicklistPath.isEmpty() || service.isEmpty()) {
        m_dynamicQuicklistImporter.reset();
    } else {
        m_dynamicQuicklistImporter.reset(new DBusMenuImporter(service, m_dynamicQuicklistPath));
        m_dynamicQuicklistImporter->updateMenu();
        if (m_dynamicQuicklistServiceWatcher == NULL) {
            m_dynamicQuicklistServiceWatcher = new QDBusServiceWatcher(this);
            m_dynamicQuicklistServiceWatcher->setConnection(QDBusConnection::sessionBus());
            connect(m_dynamicQuicklistServiceWatcher,
                    SIGNAL(serviceOwnerChanged(const QString&, const QString&, const QString&)),
                    SLOT(dynamicQuicklistImporterServiceOwnerChanged(const QString&, const QString&, const QString&)));
        }
        m_dynamicQuicklistServiceWatcher->addWatchedService(service);
    }
}

void
LauncherApplication::dynamicQuicklistImporterServiceOwnerChanged(const QString& serviceName, const QString& oldOwner, const QString& newOwner)
{
    m_dynamicQuicklistServiceWatcher->removeWatchedService(oldOwner);
    setDynamicQuicklistImporter(newOwner);
}

#include "launcherapplication.moc"
