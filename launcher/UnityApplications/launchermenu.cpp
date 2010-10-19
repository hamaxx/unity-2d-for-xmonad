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

#include <QFile>
#include <QApplication>
#include <QDesktopWidget>
#include <QPoint>
#include <QCursor>

QLauncherContextualMenu::QLauncherContextualMenu():
    QMenu(0), m_application(NULL)
{
    /* The tooltip/menu shouldn’t be modal. */
    setWindowFlags(Qt::ToolTip);

    /* Custom appearance. */
    loadCSS();

    m_title = new QAction(this);
    m_title->setEnabled(false);
    addAction(m_title);

    addSeparator();

    m_keep = new QAction(this);
    addAction(m_keep);
    m_keep->setVisible(false);
    QObject::connect(m_keep, SIGNAL(triggered()), this, SLOT(onKeepTriggered()));

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
QLauncherContextualMenu::loadCSS()
{
    /* FIXME: surely there must be a cleaner way to do that in Qt… */
    QString path;
    if (QCoreApplication::applicationDirPath() == INSTALL_PREFIX "/bin")
    {
        /* Running installed */
        path = INSTALL_PREFIX "/" UNITY_QT_DIR "/launcher/";
    }
    else
    {
        /* Uninstalled */
        path = "UnityApplications/";
    }
    path += "launchermenu.css";

    QFile file(path);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return;

    QString css(file.readAll());
    file.close();
    setStyleSheet(css);
}

void
QLauncherContextualMenu::show(int y, const QVariant& application)
{
    m_application = (QLauncherApplication*) application.value<QObject*>();

    QDesktopWidget* desktop = QApplication::desktop();
    const QRect available = desktop->availableGeometry(this);
    int height = actionGeometry(m_title).height();
    move(available.x(), y + available.y() - height / 2);
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
    m_quit->setVisible(false);
}

void
QLauncherContextualMenu::leaveEvent(QEvent* event)
{
    /* Since our menu is not a modal popup, it doesn’t capture all events, and
       thus doesn’t know when e.g. the mouse was clicked outside of its window
       (which should close it).
       Re-implementing leaveEvent(…) is a cheap workaround: hide the menu when
       the cursor leaves it. This is not the same behaviour as in unity, but it
       will do for now… */
    QDesktopWidget* desktop = QApplication::desktop();
    const QRect available = desktop->availableGeometry(this);
    QPoint cursor = QCursor::pos();
    if (cursor.x() <= available.x())
        return;

    hide(true);
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

