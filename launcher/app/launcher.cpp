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
#include <QDeclarativeContext>

#include "config.h"
#include "launcherview.h"

int main(int argc, char *argv[])
{
    /* UnityApplications plugin uses GTK APIs to retrieve theme icons
       (gtk_icon_theme_get_default) and requires a call to gtk_init */
    gtk_init(&argc, &argv);

    QApplication::setGraphicsSystem("raster");
    QApplication application(argc, argv);

    LauncherView view;
    view.setAttribute(Qt::WA_X11NetWmWindowTypeDock);
    /* FIXME: possible optimisations */
//    view.setAttribute(Qt::WA_OpaquePaintEvent);
//    view.setAttribute(Qt::WA_NoSystemBackground);
    view.setResizeMode(QDeclarativeView::SizeRootObjectToView);
    view.setFocus();

    view.engine()->addImportPath(unityQtImportPath());
    /* Note: baseUrl seems to be picky: if it does not end with a slash,
       setSource() will fail */
    view.engine()->setBaseUrl(QUrl::fromLocalFile(unityQtDirectory() + "/launcher/"));

    view.rootContext()->setContextProperty("launcherView", &view);
    view.setSource(QUrl("./Launcher.qml"));

    view.show();
    QDesktopWidget* desktop = QApplication::desktop();
    view.workAreaResized(desktop->screenNumber(&view));
    QObject::connect(desktop, SIGNAL(workAreaResized(int)), &view, SLOT(workAreaResized(int)));

    return application.exec();
}

