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

LauncherContextualMenu::LauncherContextualMenu():
    QMenu(0)
{
    /* The tooltip/menu shouldn’t be modal. */
    setWindowFlags(Qt::ToolTip);

    /* Custom appearance. */
    loadCSS();

    m_title = new QAction(this);
    m_title->setEnabled(false);
    addAction(m_title);
}

LauncherContextualMenu::~LauncherContextualMenu()
{
}

void
LauncherContextualMenu::loadCSS()
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
LauncherContextualMenu::setTitle(QString title)
{
    m_title->setText(title);
}

void
LauncherContextualMenu::prependAction(QAction* action)
{
    /* Insert an action before the title, and move the menu accordingly so that
       the title remains in the same position. */
    int y0 = actionGeometry(m_title).y();
    insertAction(m_title, action);
    int y1 = actionGeometry(m_title).y();
    move(x(), y() - y1 + y0);
}

