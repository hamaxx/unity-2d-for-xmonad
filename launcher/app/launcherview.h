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

#ifndef LAUNCHERVIEW
#define LAUNCHERVIEW

#include <QDeclarativeView>
#include <QUrl>
#include <QDragEnterEvent>

struct _WnckWindow;

class LauncherView : public QDeclarativeView
{
    Q_OBJECT

public:
    explicit LauncherView();
    Q_INVOKABLE QColor iconAverageColor(QUrl source, QSize size);

signals:
    void desktopFileDropped(QString path);
    void webpageUrlDropped(const QUrl& url);

private Q_SLOTS:
    void updateVisibility();
    void updateActiveWindowConnections();

private:
    QList<QUrl> getEventUrls(QDropEvent*);

    void dragEnterEvent(QDragEnterEvent *event);
    void dropEvent(QDropEvent *event);
    void dragMoveEvent(QDragMoveEvent *event);

    /* Whether the launcher is already being resized */
    bool m_resizing;

    /* Whether space at the left of the screen has already been reserved */
    bool m_reserved;

    struct _WnckWindow* m_activeWindow;
};

#endif // LAUNCHERVIEW

