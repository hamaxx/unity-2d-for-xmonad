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

#include <debug_p.h>

// libwnck
#undef signals
extern "C" {
#define WNCK_I_KNOW_THIS_IS_UNSTABLE
#include <libwnck/libwnck.h>
}

#include <X11/Xlib.h>
#include <X11/Xatom.h>

static void
updateActiveWindowConnectionsCB(GObject* screen, void* dummy, LauncherView* view)
{
    QMetaObject::invokeMethod(view, "updateActiveWindowConnections");
}

static void
updateVisibilityCB(GObject* screen, LauncherView* view)
{
    QMetaObject::invokeMethod(view, "updateVisibility");
}

static void
stateChangedCB(GObject* screen, WnckWindowState*, WnckWindowState*, LauncherView* view)
{
    QMetaObject::invokeMethod(view, "updateVisibility");
}

LauncherView::LauncherView()
: QDeclarativeView()
, m_resizing(false)
, m_reserved(false)
, m_activeWindow(0)
{
    setAcceptDrops(true);

    WnckScreen* screen = wnck_screen_get_default();

    g_signal_connect(G_OBJECT(screen), "active-window-changed", G_CALLBACK(updateActiveWindowConnectionsCB), this);
    g_signal_connect(G_OBJECT(screen), "active-workspace-changed", G_CALLBACK(updateVisibilityCB), this);

    updateActiveWindowConnections();
}

void
LauncherView::updateActiveWindowConnections()
{
    WnckScreen* screen = wnck_screen_get_default();

    if (m_activeWindow) {
        g_signal_handlers_disconnect_by_func(m_activeWindow, gpointer(updateVisibilityCB), this);
        g_signal_handlers_disconnect_by_func(m_activeWindow, gpointer(stateChangedCB), this);
        m_activeWindow = 0;
    }

    WnckWindow* window = wnck_screen_get_active_window(screen);
    if (window) {
        m_activeWindow = window;
        g_signal_connect(G_OBJECT(window), "state-changed", G_CALLBACK(stateChangedCB), this);
        g_signal_connect(G_OBJECT(window), "geometry-changed", G_CALLBACK(updateVisibilityCB), this);
        g_signal_connect(G_OBJECT(window), "workspace-changed", G_CALLBACK(updateVisibilityCB), this);
    }
}

void
LauncherView::updateVisibility()
{
    int launcherPid = getpid();

    // Compute launcherRect, adjust "left" to the position where the launcher
    // is fully visible.
    QRect launcherRect = window()->geometry();
    launcherRect.moveLeft(0);

    WnckScreen* screen = wnck_screen_get_default();
    WnckWorkspace* workspace = wnck_screen_get_active_workspace(screen);

    // Check whether a window is crossing our launcher rect
    bool crossWindow = false;
    GList* list = wnck_screen_get_windows(screen);
    for (; list; list = g_list_next(list)) {
        WnckWindow* window = WNCK_WINDOW(list->data);
        if (wnck_window_is_on_workspace(window, workspace) && wnck_window_get_pid(window) != launcherPid) {
            // Maximized window should always be considered as crossing the
            // window
            WnckWindowState state = wnck_window_get_state(window);
            if (state & WNCK_WINDOW_STATE_MAXIMIZED_HORIZONTALLY) {
                crossWindow = true;
                break;
            }

            // Not maximized => really check the window rect
            int x, y, width, height;
            wnck_window_get_geometry(window, &x, &y, &width, &height);
            QRect rect(x, y, width, height);
            if (rect.intersects(launcherRect)) {
                crossWindow = true;
                break;
            }
        }
    }

    QPoint pos = window()->pos();
    pos.setX(crossWindow ? -width() : 0);
    window()->move(pos);
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
