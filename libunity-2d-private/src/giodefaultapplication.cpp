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

// Qt
#include <QFileSystemWatcher>
#include <QDir>
#include <QTimer>

// gio
#include <gio/gio.h>
#include <gio/gdesktopappinfo.h>

static const QString MIMEAPPS_FILE = QDir::homePath() + QString("/.local/share/applications/mimeapps.list");

GioDefaultApplication::GioDefaultApplication(QObject* parent)
    : QObject(parent),
      m_contentType(""),
      m_desktopFile(""),
      m_defaultDesktopFile(""),
      m_mimeappsWatcher(new QFileSystemWatcher(this))
{
    /* GIO does not have any signal to inform us of changes in default
       applications to handle a certain content type. Work around that
       by monitoring file MIMEAPPS_FILE that is overwritten when default
       applications change. */
    m_mimeappsWatcher->addPath(MIMEAPPS_FILE);
    connect(m_mimeappsWatcher, SIGNAL(fileChanged(const QString&)),
                               SLOT(onMimeappsFileChanged()));

}

void GioDefaultApplication::onMimeappsFileChanged()
{
    /* FIXME: wait for 10 seconds before updating the desktop file because
       of a bug in gio where g_app_info_get_default_for_type will not immediately
       return the updated result.
       Ref.: https://bugzilla.gnome.org/show_bug.cgi?id=653999
    */
    QTimer::singleShot(10000, this, SLOT(updateDesktopFile()));
    /* If the file is already being monitored, we shouldn’t need to do anything.
       However it seems that in some cases, a change to the file will stop
       emiting further fileChanged signals, despite the file still being in the
       list of monitored files. This is the case when the desktop file is being
       edited in gedit for example. This may be a bug in QT itself.
       To work around this issue, remove the path and add it again. */
    m_mimeappsWatcher->removePath(MIMEAPPS_FILE);
    m_mimeappsWatcher->addPath(MIMEAPPS_FILE);
}

QString GioDefaultApplication::desktopFile() const
{
    return m_desktopFile;
}

QString GioDefaultApplication::contentType() const
{
    return m_contentType;
}

QString GioDefaultApplication::defaultDesktopFile() const
{
    return m_defaultDesktopFile;
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

void GioDefaultApplication::setDefaultDesktopFile(
    const QString& defaultDesktopFile)
{
    if (defaultDesktopFile == m_defaultDesktopFile) {
        return;
    }

    m_defaultDesktopFile = defaultDesktopFile;
    Q_EMIT defaultDesktopFileChanged();
    updateDesktopFile();
}

void GioDefaultApplication::updateDesktopFile()
{
    GObjectScopedPointer<GAppInfo> app_info;

    if (!m_defaultDesktopFile.isEmpty()) {
        QByteArray byte_array = m_defaultDesktopFile.toUtf8();
        gchar *desktopFile = byte_array.data();
        app_info.reset((GAppInfo*)g_desktop_app_info_new(desktopFile));
    }

    if (app_info.isNull()) {
        QByteArray byte_array = m_contentType.toUtf8();
        gchar *content_type = byte_array.data();
        app_info.reset(g_app_info_get_default_for_type(content_type, false));
    }

    if (!app_info.isNull()) {
        m_desktopFile = QString::fromUtf8(g_desktop_app_info_get_filename((GDesktopAppInfo*)app_info.data()));
    } else {
        m_desktopFile = "";
    }

    Q_EMIT desktopFileChanged();
}

#include "giodefaultapplication.moc"
