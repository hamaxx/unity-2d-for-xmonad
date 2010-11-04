/* Those have to be included before any QObject-style header to avoid
   compilation errors. */
#include <gdk/gdk.h>
#include <libwnck/libwnck.h>

#include "launcherapplication.h"
#include "bamf-matcher.h"

#include <X11/X.h>

#include <QDebug>

LauncherApplication::LauncherApplication() :
    m_application(NULL), m_appInfo(NULL), m_sticky(false), m_has_visible_window(false)
{
    QObject::connect(&m_launching_timer, SIGNAL(timeout()), this, SLOT(onLaunchingTimeouted()));
}

LauncherApplication::LauncherApplication(const LauncherApplication& other) :
    m_application(NULL), m_appInfo(NULL)
{
    /* FIXME: a number of members are not copied over */
    QObject::connect(&m_launching_timer, SIGNAL(timeout()), this, SLOT(onLaunchingTimeouted()));
    if (other.m_application != NULL)
        setBamfApplication(other.m_application);
    m_priority = other.m_priority;
}

LauncherApplication::~LauncherApplication()
{
    if(m_application != NULL)
    {
        m_application = NULL;
    }
    if(m_appInfo != NULL)
    {
        m_appInfo = NULL;
    }
}

bool
LauncherApplication::active() const
{
    if(m_application != NULL)
        return m_application->active();

    return false;
}

bool
LauncherApplication::running() const
{
    if(m_application != NULL)
        return m_application->running();

    return false;
}

bool
LauncherApplication::urgent() const
{
    if(m_application != NULL)
        return m_application->urgent();

    return false;
}

bool
LauncherApplication::sticky() const
{
    return m_sticky;
}

QString
LauncherApplication::name() const
{
    if(m_application != NULL)
        return m_application->name();

    if(m_appInfo != NULL)
        return QString::fromUtf8(g_app_info_get_name((GAppInfo*)m_appInfo));

    return QString("");
}

QString
LauncherApplication::icon() const
{
    if(m_application != NULL)
        return m_application->icon();

    if(m_appInfo != NULL)
        return QString::fromUtf8(g_icon_to_string(g_app_info_get_icon((GAppInfo*)m_appInfo)));

    return QString("");
}

QString
LauncherApplication::application_type() const
{
    if(m_application != NULL)
        return m_application->application_type();

    return QString("");
}

QString
LauncherApplication::desktop_file() const
{
    if(m_application != NULL)
        return m_application->desktop_file();

    if(m_appInfo != NULL)
        return QString::fromUtf8(g_desktop_app_info_get_filename(m_appInfo));

    return QString("");
}

void
LauncherApplication::setSticky(bool sticky)
{
    if (sticky == m_sticky)
        return;

    m_sticky = sticky;
    emit stickyChanged(sticky);
}

void
LauncherApplication::setDesktopFile(QString desktop_file)
{
    /* FIXME: should check/interact properly with an m_application != NULL */

    QByteArray byte_array = desktop_file.toUtf8();
    gchar *file = byte_array.data();

    if(desktop_file.startsWith("/"))
    {
        /* It looks like a full path to a desktop file */
        m_appInfo = g_desktop_app_info_new_from_filename(file);
    }
    else
    {
        /* It might just be a desktop file name; let GIO look for the actual
           desktop file for us */
        m_appInfo = g_desktop_app_info_new(file);
    }

    /* Emit the Changed signal on all properties that can depend on m_appInfo */
    emit desktopFileChanged(desktop_file);
    emit nameChanged(name());
    emit iconChanged(icon());
}

void
LauncherApplication::setBamfApplication(BamfApplication *application)
{
    m_application = application;

    QObject::connect(application, SIGNAL(ActiveChanged(bool)), this, SIGNAL(activeChanged(bool)));

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

    emit activeChanged(active());
    emit runningChanged(running());
    emit urgentChanged(urgent());
    emit nameChanged(name());
    emit iconChanged(icon());
    emit applicationTypeChanged(application_type());
    emit desktopFileChanged(desktop_file());
    m_launching_timer.stop();
    emit launchingChanged(launching());
    updateHasVisibleWindow();
}

void
LauncherApplication::onBamfApplicationClosed(bool running)
{
    if(running)
       return;

    //BamfApplication* application = static_cast<BamfApplication*>(sender());
    /* FIXME: should we disconnect from application's signals? */
    m_application = NULL;
    emit closed();
}

void
LauncherApplication::onDesktopFileChanged(QString desktop_file)
{
    /* FIXME: should retrieve its value in GConf */
    m_priority = 1;
}

int
LauncherApplication::priority() const
{
    return m_priority;
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
    m_has_visible_window = m_application->xids()->size() > 0;
    if (m_has_visible_window != prev)
        emit hasVisibleWindowChanged(m_has_visible_window);
}

bool
LauncherApplication::has_visible_window() const
{
    return m_has_visible_window;
}

void
LauncherApplication::activate()
{
    if (active())
        expose();

    else if (running() && has_visible_window())
        show();

    else
        launch();
}

QBool
LauncherApplication::launch()
{
    if(m_appInfo == NULL) return QBool(false);

    GError* error;
    GdkAppLaunchContext *context;
    GTimeVal timeval;

    g_get_current_time (&timeval);
    context = gdk_app_launch_context_new();
    /* Using GDK_CURRENT_TIME doesn’t seem to work, launched windows
       sometimes don’t get focus (see https://launchpad.net/bugs/643616). */
    gdk_app_launch_context_set_timestamp(context, timeval.tv_sec);

    g_app_info_launch((GAppInfo*)m_appInfo, NULL, (GAppLaunchContext*)context, &error);
    g_object_unref(context);

    /* 'launching' property becomes true for a maximum of 8 seconds and becomes
       false as soon as the application is launched */
    m_launching_timer.setSingleShot(true);
    m_launching_timer.start(8000);
    emit launchingChanged(true);

    return QBool(true);
}

void
LauncherApplication::onLaunchingTimeouted()
{
    emit launchingChanged(false);
}

void
LauncherApplication::close()
{
    if (m_application == NULL)
        return;

    BamfUintList* xids = m_application->xids();
    int size = xids->size();
    if (size < 1)
        return;

    WnckScreen* screen = wnck_screen_get_default();
    wnck_screen_force_update(screen);
    GList* windows = wnck_screen_get_windows(screen);

    /* Stop monitoring windows, this would make useless calls to
       updateHasVisibleWindow() and result in trying to invoke methods on stale
       D-Bus objects. */
    m_application->disconnect(SIGNAL(WindowAdded(BamfWindow*)));
    m_application->disconnect(SIGNAL(WindowRemoved(BamfWindow*)));

    for (int i = 0; i < size; ++i)
    {
        uint xid = xids->at(i);
        for(GList* li = windows; li != NULL; li = g_list_next(li))
        {
            WnckWindow* window = (WnckWindow*) li->data;
            if (wnck_window_get_xid(window) == xid)
            {
                wnck_window_close(window, CurrentTime);
                break;
            }
        }
    }
}

void
LauncherApplication::show()
{
    if(m_application == NULL || m_application->xids()->size() < 1) return;

    /* FIXME: pick the most important window, not just the first one */
    uint xid = m_application->xids()->at(0);

    WnckScreen* screen = wnck_screen_get_default();
    wnck_screen_force_update(screen);
    GList* windows = wnck_screen_get_windows(screen);

    for(GList* li = windows; li != NULL; li = g_list_next(li))
    {
        WnckWindow* window = (WnckWindow*) li->data;
        if (wnck_window_get_xid(window) == xid)
        {
            WnckWorkspace* workspace = wnck_window_get_workspace(window);
            wnck_workspace_activate(workspace, CurrentTime);
            wnck_window_activate(window, CurrentTime);
            break;
        }
    }
}

void
LauncherApplication::expose()
{
    /* IMPLEMENT ME: see unity’s expose manager */
    qDebug() << "FIXME: Expose mode not implemented yet.";
}

void
LauncherApplication::createMenuActions()
{
    bool is_running = running();

    QAction* keep = new QAction(m_menu);
    keep->setCheckable(is_running);
    keep->setChecked(sticky());
    keep->setText(is_running ? tr("Keep In Launcher") : tr("Remove From Launcher"));
    m_menu->addAction(keep);
    QObject::connect(keep, SIGNAL(triggered()), this, SLOT(onKeepTriggered()));

    if (is_running)
    {
        QAction* quit = new QAction(m_menu);
        quit->setText(tr("Quit"));
        m_menu->addAction(quit);
        QObject::connect(quit, SIGNAL(triggered()), this, SLOT(onQuitTriggered()));
    }
}

void
LauncherApplication::onKeepTriggered()
{
    QAction* keep = static_cast<QAction*>(sender());
    bool sticky = keep->isChecked();
    hideMenu(true);
    setSticky(sticky);
}

void
LauncherApplication::onQuitTriggered()
{
    hideMenu(true);
    close();
}

