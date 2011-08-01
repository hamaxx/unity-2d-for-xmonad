/*
 * This file is part of unity-2d
 *
 * Copyright 2011 Canonical Ltd.
 *
 * Authors:
 * - Aurélien Gâteau <aurelien.gateau@canonical.com>
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
// Self
#include <gimageutils.h>

// Local
#include <debug_p.h>

// Qt
#include <QByteArray>
#include <QImage>

// GTK
#include <gtk/gtk.h>

namespace GImageUtils
{

QImage imageForIconString(const QString& name, int size, GtkIconTheme* theme)
{
    if (!theme) {
        theme = gtk_icon_theme_get_default();
    }

    /* Load the icon by creating a GIcon from the string icon_name.
       icon_name can contain more than a simple icon name but possibly
       a string as returned by g_icon_to_string().
    */
    QByteArray utf8Name = name.toUtf8();
    GIcon *g_icon = g_icon_new_for_string(utf8Name.data(), NULL);
    GtkIconInfo *icon_info = gtk_icon_theme_lookup_by_gicon(theme, g_icon,
                                                            size,
                                                            (GtkIconLookupFlags)0);
    g_object_unref(g_icon);

    if (icon_info == NULL) {
        UQ_WARNING << "Failed to find icon:" << name;
        return QImage();
    }

    GdkPixbuf *pixbuf = gtk_icon_info_load_icon(icon_info, NULL);
    gtk_icon_info_free(icon_info);

    if (pixbuf == NULL) {
        UQ_WARNING << "Failed to load icon:" << name;
        return QImage();
    }

    QImage image = imageForPixbuf(pixbuf);
    g_object_unref(pixbuf);

    return image;
}

QImage imageForPixbuf(const GdkPixbuf* pixbuf)
{
    QImage image(gdk_pixbuf_get_pixels(pixbuf),
                   gdk_pixbuf_get_width(pixbuf),
                   gdk_pixbuf_get_height(pixbuf),
                   gdk_pixbuf_get_rowstride(pixbuf),
                   QImage::Format_ARGB32);

#if Q_BYTE_ORDER == Q_LITTLE_ENDIAN
    /* ABGR → ARGB */
    QImage swappedImage = image.rgbSwapped();
#else
    /* ABGR → BGRA */
    /* Reference: https://bugs.launchpad.net/unity-2d/+bug/758782 */
    QImage swappedImage(image.size(), image.format());
    for (int i = 0; i < swappedImage.height(); ++i) {
        QRgb* p = (QRgb*) image.constScanLine(i);
        QRgb* q = (QRgb*) swappedImage.scanLine(i);
        QRgb* end = p + image.width();
        while (p < end) {
            *q = qRgba(qAlpha(*p), qRed(*p), qGreen(*p), qBlue(*p));
            p++;
            q++;
        }
    }
#endif

    return swappedImage;
}

} // namespace
