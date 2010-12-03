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
#include "launcheritem.h"

#include <QAction>
#include <QFile>
#include <QApplication>
#include <QResizeEvent>
#include <QBitmap>
#include <QX11Info>

LauncherContextualMenu::LauncherContextualMenu():
    QMenu(0), m_folded(true), m_launcherItem(NULL), m_titleAction(NULL)
{
    /* Timer used for to hide the menu after a given delay (hideWithDelay()) */
    m_hidingDelayTimer.setSingleShot(true);
    QObject::connect(&m_hidingDelayTimer, SIGNAL(timeout()), this, SLOT(hide()));

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

    /* First action used to display the title of the item */
    m_titleAction = new QAction(this);
    m_titleAction->setEnabled(false);
    addAction(m_titleAction);
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

QString
LauncherContextualMenu::title() const
{
    return m_title;
}

void
LauncherContextualMenu::setTitle(const QString& title)
{
    m_title = title;
    /* Escaping ampersands so that they are not considered as keyboard
       accelerators. */
    m_titleAction->setText(m_title.replace("&", "&&"));
    emit titleChanged(m_title);
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
LauncherContextualMenu::transparencyAvailable() const
{
    return QX11Info::isCompositingManagerRunning();
}

void
LauncherContextualMenu::leaveEvent(QEvent* event)
{
    /* Prepare for hiding the menu when the mouse leaves it */
    m_hidingDelayTimer.start();
    QMenu::leaveEvent(event);
}

void
LauncherContextualMenu::enterEvent(QEvent* event)
{
    /* Always keep the menu visible when the mouse hovers it */
    m_hidingDelayTimer.stop();
    QMenu::enterEvent(event);
}

void
LauncherContextualMenu::show(int x, int y)
{
    m_hidingDelayTimer.stop();

    if (isVisible())
        return;

    move(x, y - minimumSize().height() / 2);
    QMenu::show();
}

void
LauncherContextualMenu::hide()
{
    /* Fold the menu upon hiding */
    setFolded(true);
    QMenu::hide();
}

void
LauncherContextualMenu::hideWithDelay(int delay)
{
    m_hidingDelayTimer.setInterval(delay);
    m_hidingDelayTimer.start();
}

bool
LauncherContextualMenu::folded() const
{
    return m_folded;
}

void
LauncherContextualMenu::setFolded(int folded)
{
    if (folded == m_folded)
        return;

    if (folded) {
        while (actions().length() > 1) {
            delete actions().takeLast();
        }
    } else {
        addSeparator();
        m_launcherItem->createMenuActions();
    }

    m_folded = folded;

    emit foldedChanged(m_folded);
}

LauncherItem*
LauncherContextualMenu::launcherItem() const
{
    return m_launcherItem;
}

void
LauncherContextualMenu::setLauncherItem(LauncherItem* launcherItem)
{
    m_launcherItem = launcherItem;
}
