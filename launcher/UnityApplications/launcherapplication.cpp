/* Those have to be included before any QObject-style header to avoid
   compilation errors. */
#include <gdk/gdk.h>
#include <libwnck/libwnck.h>

#include "launcherapplication.h"
#include "bamf-matcher.h"

#include <X11/X.h>

#include <QDebug>

QLauncherApplication::QLauncherApplication(QObject *parent) :
    QObject(parent), m_application(NULL), m_appInfo(NULL), m_sticky(false), m_has_visible_window(false)
{
    QObject::connect(&m_launching_timer, SIGNAL(timeout()), this, SLOT(onLaunchingTimeouted()));
}

QLauncherApplication::QLauncherApplication(const QLauncherApplication& other) :
    QObject(other.parent()), m_application(NULL), m_appInfo(NULL)
{
    /* FIXME: a number of members are not copied over */
    QObject::connect(&m_launching_timer, SIGNAL(timeout()), this, SLOT(onLaunchingTimeouted()));
    if (other.m_application != NULL)
        setBamfApplication(other.m_application);
    m_priority = other.m_priority;
}

QLauncherApplication::~QLauncherApplication()
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
QLauncherApplication::active() const
{
    if(m_application != NULL)
        return m_application->active();

    return false;
}

bool
QLauncherApplication::running() const
{
    if(m_application != NULL)
        return m_application->running();

    return false;
}

bool
QLauncherApplication::urgent() const
{
    if(m_application != NULL)
        return m_application->urgent();

    return false;
}

bool
QLauncherApplication::sticky() const
{
    return m_sticky;
}

QString
QLauncherApplication::name() const
{
    if(m_application != NULL)
        return m_application->name();

    if(m_appInfo != NULL)
        return QString::fromUtf8(g_app_info_get_name((GAppInfo*)m_appInfo));

    return QString("");
}

QString
QLauncherApplication::icon() const
{
    if(m_application != NULL)
        return m_application->icon();

    if(m_appInfo != NULL)
        return QString::fromUtf8(g_icon_to_string(g_app_info_get_icon((GAppInfo*)m_appInfo)));

    return QString("");
}

QString
QLauncherApplication::application_type() const
{
    if(m_application != NULL)
        return m_application->application_type();

    return QString("");
}

QString
QLauncherApplication::desktop_file() const
{
    if(m_application != NULL)
        return m_application->desktop_file();

    if(m_appInfo != NULL)
        return QString::fromUtf8(g_desktop_app_info_get_filename(m_appInfo));

    return QString("");
}

void
QLauncherApplication::setSticky(bool sticky)
{
    if (sticky == m_sticky)
        return;

    m_sticky = sticky;
    emit stickyChanged(sticky);
}

void
QLauncherApplication::setDesktopFile(QString desktop_file)
{
    /* Accept fully formed URL and truncate anything else that is not the actual
       path. Only supports local URLs (e.g. 'file:///home/john' is converted
       into '/home/john') */
//    desktop_file = QUrl(desktop_file).path();

    /* FIXME: should check/interact properly with an m_application != NULL */

    QByteArray byte_array = desktop_file.toUtf8();
    gchar *file = byte_array.data();

    m_appInfo = g_desktop_app_info_new_from_filename(file);

    /* Emit the Changed signal on all properties that can depend on m_appInfo */
    emit desktopFileChanged(desktop_file);
    emit nameChanged(name());
    emit iconChanged(icon());
}

void
QLauncherApplication::setBamfApplication(BamfApplication *application)
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
QLauncherApplication::onBamfApplicationClosed(bool running)
{
    if(running)
       return;

    //BamfApplication* application = static_cast<BamfApplication*>(sender());
    /* FIXME: should we disconnect from application's signals? */
    m_application = NULL;
    emit closed();
}

void
QLauncherApplication::onDesktopFileChanged(QString desktop_file)
{
    /* FIXME: should retrieve its value in GConf */
    m_priority = 1;
}

int
QLauncherApplication::priority() const
{
    return m_priority;
}

bool
QLauncherApplication::launching() const
{
    return m_launching_timer.isActive();
}

void
QLauncherApplication::updateHasVisibleWindow()
{
    bool prev = m_has_visible_window;
    m_has_visible_window = m_application->xids()->size() > 0;
    if (m_has_visible_window != prev)
        emit hasVisibleWindowChanged(m_has_visible_window);
}

bool
QLauncherApplication::has_visible_window() const
{
    return m_has_visible_window;
}

QBool
QLauncherApplication::launch()
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
QLauncherApplication::onLaunchingTimeouted()
{
    emit launchingChanged(false);
}

void
QLauncherApplication::close()
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
QLauncherApplication::show()
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
QLauncherApplication::expose()
{
    /* IMPLEMENT ME: see unity’s expose manager */
    qDebug() << "FIXME: Expose mode not implemented yet.";
}

