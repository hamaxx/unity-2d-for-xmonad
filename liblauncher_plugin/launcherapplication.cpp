#include "launcherapplication.h"
#include <gconf/gconf-client.h>

QLauncherApplication::QLauncherApplication(QObject *parent) :
    QObject(parent), m_application(NULL)
{
}

QLauncherApplication::QLauncherApplication(QString desktop_file) : m_application(NULL)
{
    setDesktopFile(desktop_file);
}


QLauncherApplication::~QLauncherApplication()
{
    if(m_application != NULL)
    {
        g_object_unref(m_application);
        m_application = NULL;
    }
}

bool
QLauncherApplication::running() const
{
    if(m_application == NULL) return QBool(false);

    return launcher_application_get_running(m_application);
}

QString
QLauncherApplication::name() const
{
    if(m_application == NULL) return QString("");

    const gchar *name = launcher_application_get_name(m_application);
    return QString::fromUtf8(name);
}


static gchar *
_icon_path_from_theme_get(const char *name, unsigned int size)
{
  GtkIconInfo *info;
  const gchar *filename;
  gchar *ret;

  GtkIconTheme *_theme = gtk_icon_theme_get_default();

  info = gtk_icon_theme_lookup_icon(_theme, name, size, (GtkIconLookupFlags)0);
  if (!info)
    return NULL;

  filename = gtk_icon_info_get_filename(info);
  if (g_file_test(filename,  G_FILE_TEST_EXISTS))
    ret = g_strdup(filename);
  else
    ret = NULL;

  gtk_icon_info_free(info);

  return ret;
}

QString
QLauncherApplication::icon_name() const
{
    if(m_application == NULL) return QString("");

    return QString(launcher_application_get_icon_name(m_application));
}

QUrl
QLauncherApplication::icon_path() const
{
    if(m_application == NULL) return QUrl("");

    const gchar* name = launcher_application_get_icon_name(m_application);
    if(name == NULL)
        return QUrl("");
    /* FIXME: hardcoded icon size value */
    return QUrl(QString(_icon_path_from_theme_get(name, 70)));
}


QString
QLauncherApplication::desktop_file() const
{
    if(m_application == NULL) return QString("");

    return QString(launcher_application_get_desktop_file(m_application));
}

void
QLauncherApplication::setDesktopFile(QString desktop_file)
{
    if(desktop_file == "") return;

    QByteArray byte_array = desktop_file.toUtf8();
    gchar *file = byte_array.data();

    if(m_application != NULL)
    {
        launcher_application_set_desktop_file(m_application, file);
    }
    else
    {
        LauncherAppman *appman = launcher_appman_get_default();
        m_application = launcher_appman_get_application_for_desktop_file(appman, file);

        /* FIXME: should connect to all GObject properties, not just running */
        g_signal_connect(m_application, "notify::running", G_CALLBACK(runningChangedStatic), this);

        /* FIXME: should emit the Changed signal on all properties because m_application is new */
        emit desktopFileChanged();
        emit iconPathChanged();
    }
}

QBool
QLauncherApplication::launch()
{
    if(m_application == NULL) return QBool(false);

    GError *error = NULL;
    return (QBool)launcher_application_launch(m_application, &error);
}

void
QLauncherApplication::show()
{
    if(m_application == NULL) return;

    launcher_application_show(m_application);
}

void
QLauncherApplication::removeFromConnectionsList(QString id)
{
    GConfClient *client = gconf_client_get_default();

    /* Remove item with 'id' from GConf stored connections list */
    GSList *connections = gconf_client_get_list(client,
                                                "/apps/hedley_launcher/connections/list",
                                                GCONF_VALUE_STRING,
                                                NULL);

    for(GSList *l = connections; l; l = g_slist_next(l))
    {
        gchar *d = (gchar*)l->data;
        if(QString(d) == id)
        {
            connections = g_slist_remove(connections, d);
            g_free (d);
            break;
        }
    }

    gconf_client_set_list(client,
                          "/apps/hedley_launcher/connections/list",
                          GCONF_VALUE_STRING,
                          connections,
                          NULL);

    g_slist_foreach(connections, (GFunc)g_free, NULL);
    g_slist_free(connections);

    /* Delete GConf key corresponding to item with 'id' */
    QString key = "/apps/hedley_launcher/connections/" + id;
    QByteArray byte_array = key.toUtf8();
    gconf_client_recursive_unset(client, byte_array.data(),
                                 GCONF_UNSET_INCLUDING_SCHEMA_NAMES, NULL);

    g_object_unref(client);
}
