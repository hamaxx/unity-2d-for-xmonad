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

void LauncherView::dragEnterEvent(QDragEnterEvent *event)
{
    // Check that data has a list of URLs and that at least one is
    // a desktop file.
    if (!event->mimeData()->hasUrls()) return;

    foreach (QUrl url, event->mimeData()->urls()) {
        if (url.scheme() == "file" && url.path().endsWith(".desktop")) {
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

    foreach (QUrl url, event->mimeData()->urls()) {
        if (url.scheme() == "file" && url.path().endsWith(".desktop")) {
            emit desktopFileDropped(url.path());
            accepted = true;
        }
    }

    if (accepted) event->accept();
}

/* Calculates both the background color and the glow color of a launcher tile
   based on the colors in the specified icon (using the same algorithm as Unity).
   The values are returned as list where the first item is the background color
   and the second one is the glow color
*/
QList<QVariant>
LauncherView::getColorsFromIcon(QUrl source, QSize size) const
{
    /* FIXME: we are loading again an icon that was already loaded */
    QImage icon = engine()->imageProvider("icons")->requestImage(source.path().mid(1), &size, size);

    if (icon.width() == 0 || icon.height() == 0)
    {
        qWarning() << "Unable to load icon at" << source;
        return QList<QVariant>();
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

    QList<QVariant> result;
    result.append(QVariant::fromValue(color));
    result.append(QVariant::fromValue(color));
    return result;
}
