/*
 * Copyright (C) 2011 Canonical, Ltd.
 *
 * Authors:
 *  Olivier Tilloy <olivier.tilloy@canonical.com>
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

#include "dragitemwithurl.h"

// libunity-2d
#include <gscopedpointer.h>

// GIO
extern "C" {
    #include <gio/gdesktopappinfo.h>
}

// Qt
#include <QMimeData>

DeclarativeDragItemWithUrl::DeclarativeDragItemWithUrl(QDeclarativeItem* parent)
    : DeclarativeDragItem(parent)
{
}

DeclarativeDragItemWithUrl::~DeclarativeDragItemWithUrl()
{
}

const QString& DeclarativeDragItemWithUrl::url() const
{
    return m_url;
}

void DeclarativeDragItemWithUrl::setUrl(const QString& url)
{
    if (url != m_url) {
        m_url = url;
        Q_EMIT urlChanged(m_url);
    }
}

QMimeData* DeclarativeDragItemWithUrl::mimeData() const
{
    QMimeData* data = new QMimeData;
    if (!m_url.isEmpty()) {
        QList<QUrl> urls;
        urls.append(decodeUri(m_url));
        data->setUrls(urls);
    }
    return data;
}

QUrl DeclarativeDragItemWithUrl::decodeUri(const QString& uri)
{
    if (uri.startsWith("application://")) {
        QString desktopFileName = uri.mid(14);
        QByteArray bytes = desktopFileName.toUtf8();
        GObjectScopedPointer<GDesktopAppInfo> appInfo(g_desktop_app_info_new(bytes.constData()));
        if (appInfo.isNull()) {
            return QUrl(uri);
        }
        QString filePath = QString::fromUtf8(g_desktop_app_info_get_filename(appInfo.data()));
        return QUrl("file://" + filePath);
    } else {
        return QUrl(uri);
    }
}

#include "dragitemwithurl.moc"
