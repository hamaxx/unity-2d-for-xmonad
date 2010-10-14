#include <gtk/gtk.h>
#include <gdk-pixbuf/gdk-pixbuf.h>
#include "iconimageprovider.h"

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

    QByteArray byte_array = icon_name.toUtf8();
    gchar *g_icon_name = byte_array.data();

    /* gtk_icon_theme_load_icon for a given size will sometimes return path
       to an icon of a smaller size */
    GdkPixbuf *pixbuf = gtk_icon_theme_load_icon(theme, g_icon_name,
                                                 requestedSize.width(),
                                                 (GtkIconLookupFlags)0, NULL);

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
