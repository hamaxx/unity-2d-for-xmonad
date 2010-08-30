#include "launcherapplication.h"

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
QLauncherApplication::running() const
{
    if(m_appInfo == NULL) return QBool(false);

//    return launcher_application_get_running(m_appInfo);
}

QString
QLauncherApplication::name() const
{
    if(m_appInfo == NULL) return QString("");

//    const gchar *name = launcher_application_get_name(m_appInfo);
//    return QString::fromUtf8(name);
}


QString
QLauncherApplication::icon_name() const
{
    if(m_appInfo == NULL) return QString("");

    return g_icon_to_string(g_app_info_get_icon((GAppInfo*)m_appInfo));

/*    const gchar* name = launcher_application_get_icon_name(m_appInfo);
    if(name == NULL)
        return QString("");
    else
        return QString::fromUtf8(name);*/
}


QString
QLauncherApplication::desktop_file() const
{
    if(m_appInfo == NULL) return QString("");

//    return QString::fromUtf8(launcher_application_get_desktop_file(m_appInfo));
}

QString
QLauncherApplication::type() const
{
    if(m_appInfo == NULL) return QString("");
}

void
QLauncherApplication::setDesktopFile(QString desktop_file)
{
    /* Accept fully formed URL and truncate anything else that is not the actual
       path. Only supports local URLs (e.g. 'file:///home/john' is converted
       into '/home/john') */
//    desktop_file = QUrl(desktop_file).path();

    QByteArray byte_array = desktop_file.toUtf8();
    gchar *file = byte_array.data();

    m_appInfo = g_desktop_app_info_new_from_filename(file);

    /* Emit the Changed signal on all properties because m_appInfo is new */
    emit desktopFileChanged();

    emit runningChanged();
    emit nameChanged();
    emit iconNameChanged();
    emit typeChanged();
}

QBool
QLauncherApplication::launch()
{
    if(m_appInfo == NULL) return QBool(false);

//    GError *error = NULL;
//    return (QBool)launcher_application_launch(m_appInfo, parameter.toUtf8().data(), &error);
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
