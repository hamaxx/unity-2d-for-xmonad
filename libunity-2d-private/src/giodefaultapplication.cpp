/*
 * Copyright (C) 2011 Canonical, Ltd.
 *
 * Authors:
 *  Florian Boucault <florian.boucault@canonical.com>
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

#include "giodefaultapplication.h"

// libunity-2d
#include <gscopedpointer.h>

// gio
#include <gio/gio.h>
#include <gio/gdesktopappinfo.h>


GioDefaultApplication::GioDefaultApplication(QObject* parent)
    : QObject(parent),
      m_contentType(""),
      m_desktopFile("")
{
    /* FIXME: implement change notification by monitoring ~/.local/share/applications/mimeapps.list */
}

QString GioDefaultApplication::desktopFile() const
{
    return m_desktopFile;
}

QString GioDefaultApplication::contentType() const
{
    return m_contentType;
}

void GioDefaultApplication::setContentType(const QString& contentType)
{
    if (contentType == m_contentType) {
        return;
    }

    m_contentType = contentType;
    Q_EMIT contentTypeChanged();
    updateDesktopFile();
}

void GioDefaultApplication::updateDesktopFile()
{
    GObjectScopedPointer<GAppInfo> app_info;
    QByteArray byte_array = m_contentType.toUtf8();
    gchar *content_type = byte_array.data();

    app_info.reset(g_app_info_get_default_for_type(content_type, false));
    if (!app_info.isNull()) {
        m_desktopFile = QString::fromUtf8(g_desktop_app_info_get_filename((GDesktopAppInfo*)app_info.data()));
    } else {
        m_desktopFile = "";
    }

    Q_EMIT desktopFileChanged();
}

#include "giodefaultapplication.moc"
