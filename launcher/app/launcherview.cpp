/*
 * Copyright (C) 2010 Canonical, Ltd.
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

#include "launcherview.h"

#include <QApplication>
#include <QDesktopWidget>
#include <QX11Info>
#include <QDebug>

#include <QtDeclarative/qdeclarative.h>
#include <QDeclarativeEngine>
#include <QDeclarativeContext>
#include <QDeclarativeImageProvider>

#include <X11/Xlib.h>
#include <X11/Xatom.h>

LauncherView::LauncherView() :
    QDeclarativeView(), m_resizing(false), m_reserved(false)
{
    setAcceptDrops(true);
}

QList<QUrl>
LauncherView::getEventUrls(DeclarativeDragDropEvent* event)
{
    const DeclarativeMimeData* mimeData = event->mimeData();
    QList<QUrl> result;
    QStringList urls = mimeData->urls();
    if (!urls.isEmpty()) {
        Q_FOREACH(QString url, urls) {
            result.append(QUrl::fromEncoded(url.toUtf8()));
        }
    } else {
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
            result.append(url);
        }
    }
    return result;
}

bool
LauncherView::onDragEnter(QObject* event)
{
    DeclarativeDragDropEvent* dde = qobject_cast<DeclarativeDragDropEvent*>(event);
    /* Check that data has a list of URLs and that at least one is either a
       desktop file or a web page. */
    QList<QUrl> urls = getEventUrls(dde);

    if (urls.isEmpty()) {
        return false;
    }

    Q_FOREACH(QUrl url, urls) {
        if ((url.scheme() == "file" && url.path().endsWith(".desktop")) ||
            url.scheme().startsWith("http")) {
            dde->setAccepted(true);
            return true;
        }
    }

    return false;
}

bool
LauncherView::onDragLeave(QObject* event)
{
    Q_UNUSED(event)
    return false;
}

bool
LauncherView::onDrop(QObject* event)
{
    DeclarativeDragDropEvent* dde = qobject_cast<DeclarativeDragDropEvent*>(event);
    bool accepted = false;
    bool handled = false;

    QList<QUrl> urls = getEventUrls(dde);
    Q_FOREACH(QUrl url, urls) {
        if (url.scheme() == "file" && url.path().endsWith(".desktop")) {
            emit desktopFileDropped(url.path());
            accepted = true;
            handled = true;
        }
        else if (url.scheme().startsWith("http")) {
            emit webpageUrlDropped(url);
            accepted = true;
            handled = true;
        }
    }

    if (accepted) {
        dde->setAccepted(true);
    }

    return handled;
}

QColor
LauncherView::iconAverageColor(QUrl source, QSize size)
{
    /* FIXME: we are loading again an icon that was already loaded */
    QImage icon = engine()->imageProvider("icons")->requestImage(source.path().mid(1), &size, size);

    if (icon.width() == 0 || icon.height() == 0)
    {
        qWarning() << "Unable to load icon at" << source;
        return QColor();
    }

    int total_r = 0, total_g = 0, total_b = 0;
    int select_r = 0, select_g = 0, select_b = 0;
    int selected_pixels = 0;

    for (int y=0; y<icon.height(); ++y)
    {
        for (int x=0; x<icon.width(); ++x)
        {
            QColor color = QColor::fromRgba(icon.pixel(x, y));

            if (color.alphaF() < 0.5)
                continue;

            total_r += color.red();
            total_g += color.green();
            total_b += color.blue();

            if (color.saturationF() <= 0.33)
                continue;

            select_r += color.red();
            select_g += color.green();
            select_b += color.blue();

            selected_pixels++;
        }
    }

    QColor color;

    if (selected_pixels <= 20)
    {
        int total_pixels = icon.width()*icon.height();
        color = QColor::fromRgb(total_r/total_pixels,
                                total_g/total_pixels,
                                total_b/total_pixels);
        color.setHsv(color.hue(), 0, color.value());
    }
    else
    {
        color = QColor::fromRgb(select_r/selected_pixels,
                                select_g/selected_pixels,
                                select_b/selected_pixels);
        float saturation = qMin(color.saturationF()*0.7, 1.0);
        float value = qMin(color.valueF()*1.4, 1.0);
        color.setHsvF(color.hueF(), saturation, value);
    }

    return color;
}
