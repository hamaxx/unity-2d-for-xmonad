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

void
LauncherView::workAreaResized(int screen)
{
    if (m_resizing)
    {
        qDebug("Second call to LauncherView::workAreaResized. Ignoring.");

        /* FIXME: this is a hack to avoid infinite recursion: reserving space
           at the left of the screen triggers the emission of the
           workAreaResized signal… */
        m_resizing = false;
        return;
    }

    QDesktopWidget* desktop = QApplication::desktop();
    if (screen == desktop->screenNumber(this))
    {
        const QRect screen = desktop->screenGeometry(this);
        const QRect available = desktop->availableGeometry(this);
        this->resize(this->size().width(), available.height());
        uint left = available.x();
        /* This assumes that we are the only panel on the left of the screen */
        if (m_reserved) left -= this->size().width();
        this->move(left, available.y());

        qDebug("LauncherView::workAreaResized: resize to (%d, %d) and move to (%d, %d)",
               this->size().width(), this->size().height(), this->x(), this->y());

        m_resizing = true;

        /* Reserve space at the left edge of the screen (the launcher is a panel) */
        Atom atom = XInternAtom(QX11Info::display(), "_NET_WM_STRUT_PARTIAL", False);
        ulong struts[12] = {left + this->size().width(), 0, 0, 0,
                           available.y(), available.y() + available.height(), 0, 0,
                           0, 0, 0, 0};
        XChangeProperty(QX11Info::display(), this->effectiveWinId(), atom,
                        XA_CARDINAL, 32, PropModeReplace,
                        (unsigned char *) &struts, 12);
        m_reserved = true;
    }
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
