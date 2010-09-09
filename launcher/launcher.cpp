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

#include <gtk/gtk.h>
#include <QApplication>
#include <QDesktopWidget>
#include <QDeclarativeEngine>

#include "launcherview.h"

int main(int argc, char *argv[])
{
    /* UnityApplications plugin uses GTK APIs to retrieve theme icons
       (gtk_icon_theme_get_defaul) and requires a call to gtk_init */
    gtk_init(&argc, &argv);

    QApplication application(argc, argv);

    LauncherView view;
    view.setAttribute(Qt::WA_X11NetWmWindowTypeDock);
    /* FIXME: possible optimisations */
//    view.setAttribute(Qt::WA_OpaquePaintEvent);
//    view.setAttribute(Qt::WA_NoSystemBackground);
    view.setResizeMode(QDeclarativeView::SizeRootObjectToView);
    view.setFocus();

    if (QCoreApplication::applicationDirPath() == "/usr/bin")
    {
        /* Running installed */
        view.engine()->setBaseUrl(QUrl::fromLocalFile("/usr/share/unity-qt/"));
    }
    else
    {
        /* Uninstalled: make sure local plugins such as UnityApplications are
           importable */
        view.engine()->addImportPath(QString("."));
    }

    /* This is showing the whole unity desktop, not just the launcher: */
    view.setSource(QUrl("./Launcher.qml"));

    view.show();
    QDesktopWidget* desktop = QApplication::desktop();
    view.workAreaResized(desktop->screenNumber(&view));
    QObject::connect(desktop, SIGNAL(workAreaResized(int)), &view, SLOT(workAreaResized(int)));

    return application.exec();
}

