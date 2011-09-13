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

#include <gtk/gtk.h>
#include <gdk-pixbuf/gdk-pixbuf.h>
#include "iconimageprovider.h"

#include "config.h"

#include <QFile>

#include <debug_p.h>
#include <gimageutils.h>

static const char* UNITY_RES_PATH = "/usr/share/unity/";

IconImageProvider::IconImageProvider() : QDeclarativeImageProvider(QDeclarativeImageProvider::Image)
{
}

IconImageProvider::~IconImageProvider()
{
    /* unreference cached themes */
    Q_FOREACH(void* theme, m_themes.values()) {
        g_object_unref((GtkIconTheme*)theme);
    }
}

QImage IconImageProvider::requestImage(const QString &id, QSize *size, const QSize &requestedSize)
{
    /* Special case handling for image resources that belong to the unity
       package. If unity is not installed, as a fallback we rewrite the path to
       try and locate them in our (unity-2d) resource directory.
       See https://launchpad.net/bugs/672450 for a discussion. */
    /* It is not using common facilities from config.h because the last attempt
       to do so was a failure due to the fragility of the code path. For example
       it is very easy to break the entire mechanism by adding or forgetting a
       slash in any of the paths. */
    QString iconFilePath;
    if (id.startsWith(UNITY_RES_PATH)) {
        iconFilePath = id;
        if (!QFile::exists(iconFilePath)) {
            iconFilePath.replace(UNITY_RES_PATH, INSTALL_PREFIX "/share/unity-2d/");
        }
    } else if (id.startsWith("/")) {
        iconFilePath = id;
    }

    /* We have a direct path to the icon file. Let's load it, scale it if required and
       we are done */
    if (!iconFilePath.isEmpty()) {
        QImage icon(iconFilePath);
        if (icon.isNull()) {
            UQ_WARNING << "Failed to directly load icon at path:" << iconFilePath;
            return QImage();
        }

        if (requestedSize.isValid()) {
            icon = icon.scaled(requestedSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        }

        if (size) {
            *size = icon.size();
        }
        return icon;
    }

    /* if id is of the form theme_name/icon_name then lookup the icon in the
       specified theme otherwise in the default theme */
    QString icon_name;
    GtkIconTheme *theme;

    QStringList split_id = id.split("/");
    if(split_id.length() > 1) {
        /* use specified theme */
        QString theme_name = split_id[0];
        icon_name = split_id[1];

        if(m_themes.contains(theme_name)) {
            theme = (GtkIconTheme*)m_themes[theme_name];
        } else {
            theme = gtk_icon_theme_new();
            gtk_icon_theme_set_custom_theme(theme, theme_name.toUtf8().data());
            m_themes[theme_name] = theme;
        }
    } else {
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
        || icon_name.endsWith(".jpg")) {
        icon_name.chop(4);
    }

    QImage image = GImageUtils::imageForIconString(icon_name, requestedSize.width(), theme);
    if (size) {
        *size = image.size();
    }
    return image;
}
