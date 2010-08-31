#include <gdk/gdk.h>
#include "launcherapplication.h"
#include "bamf-matcher.h"

QLauncherApplication::QLauncherApplication(QObject *parent) :
    QObject(parent), m_application(NULL), m_appInfo(NULL)
{
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
    QObject::connect(application, SIGNAL(RunningChanged(bool)), this, SIGNAL(runningChanged(bool)));
    QObject::connect(application, SIGNAL(UrgentChanged(bool)), this, SIGNAL(urgentChanged(bool)));
    /* FIXME: a bug somewhere makes connecting to the Closed() signal not work even though
              the emit Closed() in bamf-view.cpp is reached. */
    QObject::connect(application, SIGNAL(RunningChanged(bool)), this, SLOT(onBamfApplicationClosed(bool)));

    emit activeChanged(active());
    emit runningChanged(running());
    emit urgentChanged(urgent());
    emit nameChanged(name());
    emit iconChanged(icon());
    emit applicationTypeChanged(application_type());
    emit desktopFileChanged(desktop_file());
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

QBool
QLauncherApplication::launch()
{
    if(m_appInfo == NULL || m_application != NULL) return QBool(false);

    GError** error;
    GdkAppLaunchContext *context;
//    GTimeVal timeval;

//    g_get_current_time (&timeval);
    context = gdk_app_launch_context_new();
    gdk_app_launch_context_set_screen(context, gdk_screen_get_default());
//    gdk_app_launch_context_set_timestamp(context, timeval.tv_sec);
    gdk_app_launch_context_set_timestamp(context, GDK_CURRENT_TIME);

    g_app_info_launch((GAppInfo*)m_appInfo, NULL, (GAppLaunchContext*)context, error);
}

void
QLauncherApplication::close()
{
    if(m_appInfo == NULL) return;

//    launcher_application_close(m_appInfo);
}

void
QLauncherApplication::show()
{
    if(m_appInfo == NULL) return;

//    launcher_application_show(m_appInfo);
}
