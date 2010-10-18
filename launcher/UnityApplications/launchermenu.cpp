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

#include "launchermenu.h"

#include <QApplication>
#include <QDesktopWidget>
#include <QPoint>
#include <QCursor>

QLauncherContextualMenu::QLauncherContextualMenu():
    QMenu(0), m_application(NULL)
{
    /* The tooltip/menu shouldn’t be modal. */
    setWindowFlags(Qt::ToolTip);

    m_title = new QAction(this);
    m_title->setEnabled(false);
    addAction(m_title);

    addSeparator();

    m_keep = new QAction(this);
    addAction(m_keep);
    m_keep->setVisible(false);
    QObject::connect(m_keep, SIGNAL(triggered()), this, SLOT(onKeepTriggered()));

    m_separator = addSeparator();
    m_separator->setVisible(false);

    m_quit = new QAction(this);
    m_quit->setText("Quit");
    m_quit->setVisible(false);
    addAction(m_quit);
    QObject::connect(m_quit, SIGNAL(triggered()), this, SLOT(onQuitTriggered()));
}

QLauncherContextualMenu::~QLauncherContextualMenu()
{
}

void
QLauncherContextualMenu::show(int y, const QVariant& application)
{
    m_application = (QLauncherApplication*) application.value<QObject*>();

    QDesktopWidget* desktop = QApplication::desktop();
    const QRect available = desktop->availableGeometry(this);
    move(available.x(), y + available.y());
    QWidget::show();
    /* Set the title after showing so that the width is correctly updated. */
    m_title->setText(m_application->name());
}

void
QLauncherContextualMenu::show_menu()
{
    bool running = m_application->running();

    m_keep->setText(running ? "Keep In Launcher" : "Remove From Launcher");
    m_keep->setCheckable(running);
    m_keep->setChecked(m_application->sticky());
    m_keep->setVisible(true);
    m_separator->setVisible(running);
    m_quit->setVisible(running);
}

void
QLauncherContextualMenu::hide(bool force)
{
    if (!force && m_keep->isVisible())
    {
        QDesktopWidget* desktop = QApplication::desktop();
        const QRect available = desktop->availableGeometry(this);
        QPoint cursor = QCursor::pos();
        if (cursor.x() >= available.x())
            return;
    }

    QWidget::hide();
    m_application = NULL;
    m_keep->setVisible(false);
    m_separator->setVisible(false);
    m_quit->setVisible(false);
}

void
QLauncherContextualMenu::onKeepTriggered()
{
    m_application->setSticky(m_keep->isChecked());
    hide(true);
}

void
QLauncherContextualMenu::onQuitTriggered()
{
    m_application->close();
    hide(true);
}

