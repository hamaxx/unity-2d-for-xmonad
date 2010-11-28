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

#include "config.h"
#include "launchermenu.h"

#include <QAction>
#include <QFile>
#include <QApplication>
#include <QResizeEvent>
#include <QBitmap>
#include <QX11Info>

LauncherContextualMenu::LauncherContextualMenu():
    QMenu(0)
{
    /* The tooltip/menu shouldn’t be modal. */
    setWindowFlags(Qt::ToolTip);

    /* The tooltip/menu should not move when switching workspaces. */
    setAttribute(Qt::WA_X11NetWmWindowTypeDock);

    /* Use transparency if available.
       Warning: if the availability of transparency changes over time, for
                example because a compositing window manager is launched, we
                do not react to that and the menu is likely to break visually.
                Ref: http://bugreports.qt.nokia.com/browse/QTBUG-6044
    */
    if (transparencyAvailable()) {
        setAttribute(Qt::WA_TranslucentBackground);
    }

    /* Custom appearance. */
    loadCSS();
}

LauncherContextualMenu::~LauncherContextualMenu()
{
}

void
LauncherContextualMenu::loadCSS()
{
    QString cssFilePath = unityQtDirectory() + "/launcher/launchermenu.css";

    QFile file(cssFilePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return;

    QString css(file.readAll());
    file.close();
    setStyleSheet(css);
}

void
LauncherContextualMenu::setTitle(QString title)
{
    QAction* action = new QAction(this);
    action->setEnabled(false);
    addAction(action);
    action->setText(title);
}

void
LauncherContextualMenu::resizeEvent(QResizeEvent* event)
{
    /* If transparent windows are not available use the XShape extension */
    if (!transparencyAvailable()) {
        QPixmap pixmap(event->size());
        render(&pixmap, QPoint(), QRegion(),
               QWidget::DrawWindowBackground | QWidget::DrawChildren | QWidget::IgnoreMask);
        setMask(pixmap.createMaskFromColor("red"));
    }

    QMenu::resizeEvent(event);
}

bool
LauncherContextualMenu::transparencyAvailable()
{
    return QX11Info::isCompositingManagerRunning();
}
