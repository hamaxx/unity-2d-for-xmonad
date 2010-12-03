#include <gtk/gtk.h>
#include <gdk-pixbuf/gdk-pixbuf.h>
#include "iconimageprovider.h"

#include "config.h"

#include <QFile>


static const char* UNITY_RES_PATH = "/usr/share/unity/";

IconImageProvider::IconImageProvider() : QDeclarativeImageProvider(QDeclarativeImageProvider::Image)
{
}

IconImageProvider::~IconImageProvider()
{
    /* unreference cached themes */
    foreach(void* theme, m_themes.values())
        g_object_unref((GtkIconTheme*)theme);
}

QImage IconImageProvider::requestImage(const QString &id, QSize *size, const QSize &requestedSize)
{
    /* Special case handling for image resources that belong to the unity
       package. If unity is not installed, as a fallback we rewrite the path to
       try and locate them in our (unity-qt) resource directory.
       See https://launchpad.net/bugs/672450 for a discussion. */
    /* It is not using common facilities from config.h because the last attempt
       to do so was a failure due to the fragility of the code path. For example
       it is very easy to break the entire mechanism by adding or forgetting a
       slash in any of the paths. */
    if (id.startsWith(UNITY_RES_PATH))
    {
        if (QFile::exists(id))
        {
            return QImage(id);
        }
        else
        {
            QString rid(id);
            rid.replace(UNITY_RES_PATH, INSTALL_PREFIX "/share/unity-qt/");
            /* No need to check whether the file exists, we don’t have a
               fallback anyway. */
            return QImage(rid);
        }
    }

    /* Dealing with case where id is an absolute path to the icon file */
    if(id.startsWith("/"))
        return QImage(id);

    /* if id is of the form theme_name/icon_name then lookup the icon in the
       specified theme otherwise in the default theme */
    QString icon_name;
    GtkIconTheme *theme;

    QStringList split_id = id.split("/");
    if(split_id.length()>1)
    {
        /* use specified theme */
        QString theme_name = split_id[0];
        icon_name = split_id[1];

        if(m_themes.contains(theme_name))
        {
            theme = (GtkIconTheme*)m_themes[theme_name];
        }
        else
        {
            theme = gtk_icon_theme_new();
            gtk_icon_theme_set_custom_theme(theme, theme_name.toUtf8().data());
            m_themes[theme_name] = theme;
        }
    }
    else
    {
        /* use default theme */
        theme = gtk_icon_theme_get_default();
        icon_name = id;
    }

    /* Some desktop files have a malformed Icon= key where the value contains
       not only the icon name but also an extension which makes the lookup fail.
       Solution: chop off the extension.

       See http://standards.freedesktop.org/icon-theme-spec/icon-theme-spec-latest.html
       for more details.
    */
    if (icon_name.endsWith(".png") || icon_name.endsWith(".svg")
        || icon_name.endsWith(".xpm") || icon_name.endsWith(".gif")
        || icon_name.endsWith(".jpg"))
    {
        icon_name.chop(4);
    }

    /* Load the icon by creating a GIcon from the string icon_name.
       icon_name can contain more than a simple icon name but possibly
       a string as returned by g_icon_to_string().
    */
    QByteArray byte_array = icon_name.toUtf8();
    gchar *g_icon_name = byte_array.data();

    GIcon *g_icon = g_icon_new_for_string(g_icon_name, NULL);
    GtkIconInfo *icon_info = gtk_icon_theme_lookup_by_gicon(theme, g_icon,
                                                            requestedSize.width(),
                                                            (GtkIconLookupFlags)0);
    GdkPixbuf *pixbuf = gtk_icon_info_load_icon(icon_info, NULL);

    /* FIXME: maybe an exception should be raised instead? */
    if(pixbuf == NULL) return QImage();

    QImage image(gdk_pixbuf_get_pixels(pixbuf),
                   gdk_pixbuf_get_width(pixbuf),
                   gdk_pixbuf_get_height(pixbuf),
                   gdk_pixbuf_get_rowstride(pixbuf),
                   QImage::Format_ARGB32);
    QImage swappedImage = image.rgbSwapped();
    g_object_unref(pixbuf);

    if (size)
        *size = swappedImage.size();

    return swappedImage;
}
