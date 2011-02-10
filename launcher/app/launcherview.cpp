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

#define MAX(a,b) ((a >= b) ? a : b)
#define MIN(a,b) ((a <= b) ? a : b)

LauncherView::LauncherView() :
    QDeclarativeView(), m_resizing(false), m_reserved(false)
{
    setAcceptDrops(true);
}

QList<QUrl>
LauncherView::getEventUrls(QDropEvent* event)
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

void LauncherView::dragEnterEvent(QDragEnterEvent *event)
{
    // Check that data has a list of URLs and that at least one is either
    // a desktop file or a web page.
    QList<QUrl> urls = getEventUrls(event);

    if (urls.isEmpty()) {
        return;
    }

    foreach (QUrl url, urls) {
        if ((url.scheme() == "file" && url.path().endsWith(".desktop")) ||
            url.scheme().startsWith("http")) {
            event->acceptProposedAction();
            break;
        }
    }
}

void LauncherView::dragMoveEvent(QDragMoveEvent *event)
{
    event->acceptProposedAction();
}

void LauncherView::dropEvent(QDropEvent *event)
{
    bool accepted = false;

    QList<QUrl> urls = getEventUrls(event);
    foreach (QUrl url, urls) {
        if (url.scheme() == "file" && url.path().endsWith(".desktop")) {
            emit desktopFileDropped(url.path());
            accepted = true;
        }
        else if (url.scheme().startsWith("http")) {
            emit webpageUrlDropped(url);
            accepted = true;
        }
    }

    if (accepted) event->accept();
}

/* Calculates both the background color and the glow color of a launcher tile
   based on the colors in the specified icon (using the same algorithm as Unity).
   The values are returned as list where the first item is the background color
   and the second one is the glow color.
*/
QList<QVariant>
LauncherView::getColorsFromIcon(QUrl source, QSize size) const
{
    QList<QVariant> colors;

    // FIXME: we should find a way to avoid reloading the icon
    QImage icon = engine()->imageProvider("icons")->requestImage(source.path().mid(1), &size, size);
    if (icon.width() == 0 || icon.height() == 0) {
        qWarning() << "Unable to load icon in getColorsFromIcon from" << source;
        return colors;
    }

    long int rtotal = 0, gtotal = 0, btotal = 0;
    float total = 0.0f;

    for (int y = 0; y < icon.height(); ++y) {
        for (int x = 0; x < icon.width(); ++x) {
            QColor color = QColor::fromRgba(icon.pixel(x, y));

            float saturation = (MAX (color.red(), MAX (color.green(), color.blue())) -
                                MIN (color.red(), MIN (color.green(), color.blue()))) / 255.0f;
            float relevance = .1 + .9 * (color.alpha() / 255.0f) * saturation;

            rtotal += (unsigned char) (color.red() * relevance);
            gtotal += (unsigned char) (color.green() * relevance);
            btotal += (unsigned char) (color.blue() * relevance);

            total += relevance * 255;
        }
    }

    QColor hsv = QColor::fromRgbF(rtotal / total, gtotal / total, btotal / total).toHsv();

    /* Background color is the base color with 0.90f HSV value */
    hsv.setHsvF(hsv.hueF(),
                (hsv.saturationF() > .15f) ? 0.65f : hsv.saturationF(),
                0.90f);
    colors.append(QVariant::fromValue(hsv.toRgb()));

    /* Glow color is the base color with 1.0f HSV value */
    hsv.setHsvF(hsv.hueF(), hsv.saturationF(), 1.0f);
    colors.append(QVariant::fromValue(hsv.toRgb()));

    return colors;
}
