#include <gtk/gtk.h>
#include <gdk-pixbuf/gdk-pixbuf.h>
#include "iconimageprovider.h"


IconImageProvider::IconImageProvider() : QDeclarativeImageProvider()
{
}

/* FIXME: with newer Qt request was deprecated in favor of requestPixmap and requestImage.
          a new constructor parameter was added to determine which type the Provider is:

          : QDeclarativeImageProvider(QDeclarativeImageProvider::Pixmap)
*/
QImage IconImageProvider::request(const QString &id, QSize *size, const QSize &requestedSize)
{
    /* Dealing with case where id is an absolute path to the icon file */
    if(id.startsWith("/"))
        return QImage(id);

    QByteArray byte_array = id.toUtf8();
    gchar *icon_name = byte_array.data();

    /* gtk_icon_theme_load_icon for a given size will sometimes return path
       to an icon of a smaller size */
    GtkIconTheme *theme = gtk_icon_theme_get_default();
    GdkPixbuf *pixbuf = gtk_icon_theme_load_icon(theme, icon_name,
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
