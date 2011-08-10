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

#include "launcherdropitem.h"

#include <QGraphicsSceneDragDropEvent>
#include <QMimeData>

LauncherDropItem::LauncherDropItem(QDeclarativeItem *parent) : DeclarativeDropItem(parent)
{
}

void LauncherDropItem::dragEnterEvent(QGraphicsSceneDragDropEvent *event)
{
    Q_FOREACH(QUrl url, getEventUrls(event)) {
        if ((url.scheme() == "file" && url.path().endsWith(".desktop")) ||
            url.scheme().startsWith("http")) {
            event->setAccepted(true);
            return;
        }
    }
    event->setAccepted(false);
}

void LauncherDropItem::dropEvent(QGraphicsSceneDragDropEvent *event)
{
    Q_FOREACH(QUrl url, getEventUrls(event)) {
        if (url.scheme() == "file" && url.path().endsWith(".desktop")) {
            desktopFileDropped(url.path());
        } else if (url.scheme().startsWith("http")) {
            webpageUrlDropped(url);
        }
    }
    event->setAccepted(false);
}

QList<QUrl>
LauncherDropItem::getEventUrls(QGraphicsSceneDragDropEvent* event)
{
    const QMimeData* mimeData = event->mimeData();
    if (mimeData->hasUrls()) {
        return mimeData->urls();
    }
    else if (mimeData->hasText()) {
        /* When dragging an URL from firefox’s address bar, it is properly
           recognized as such by the event. However, the same doesn’t work
           for chromium: the URL is recognized as plain text.
           We cope with this unfriendly behaviour by trying to build a URL out
           of the text. This assumes there’s only one URL. */
        QString text = mimeData->text();
        QUrl url(text);
        if (url.isRelative()) {
            /* On top of that, chromium sometimes chops off the scheme… */
            url = QUrl("http://" + text);
        }
        if (url.isValid()) {
            QList<QUrl> urls;
            urls.append(url);
            return urls;
        }
    }
    return QList<QUrl>();
}

#include "launcherdropitem.moc"
