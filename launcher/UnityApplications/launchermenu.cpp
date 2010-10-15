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

QLauncherContextualMenu::QLauncherContextualMenu():
    QWidget(0, Qt::FramelessWindowHint), m_application(NULL)
{
    m_layout = new QVBoxLayout;
    setLayout(m_layout);

    m_title = new QLabel;
    m_layout->addWidget(m_title);

    m_menu = new QMenu(this);
    /* Unset the modal character of the menu, as it is embedded. */
    m_menu->setWindowFlags(Qt::Widget);
    m_layout->addWidget(m_menu);

    m_keep = new QAction(this);
    m_menu->addAction(m_keep);
    QObject::connect(m_keep, SIGNAL(triggered()), this, SLOT(onKeepTriggered()));

    m_separator = m_menu->addSeparator();

    m_quit = new QAction(this);
    m_quit->setText("Quit");
    m_menu->addAction(m_quit);
    QObject::connect(m_quit, SIGNAL(triggered()), this, SLOT(onQuitTriggered()));
}

QLauncherContextualMenu::~QLauncherContextualMenu()
{
}

void
QLauncherContextualMenu::show(int y, const QVariant& application)
{
    m_application = (QLauncherApplication*) application.value<QObject*>();
    bool running = m_application->running();

    m_title->setText(m_application->name());
    m_keep->setText(running ? "Keep In Launcher" : "Remove From Launcher");
    m_keep->setCheckable(running);
    m_keep->setChecked(m_application->sticky());
    m_separator->setVisible(running);
    m_quit->setVisible(running);

    QDesktopWidget* desktop = QApplication::desktop();
    const QRect available = desktop->availableGeometry(this);
    move(available.x(), y + available.y() - m_title->sizeHint().height() / 2);
    QWidget::show();
}

void
QLauncherContextualMenu::onKeepTriggered()
{
    m_application->setSticky(m_keep->isChecked());
}

void
QLauncherContextualMenu::onQuitTriggered()
{
    m_application->close();
}

