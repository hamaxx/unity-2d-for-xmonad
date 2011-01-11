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
#include <QDir>

#include "config.h"
#include "launcherview.h"
#include "launchercontrol.h"

int main(int argc, char *argv[])
{
    /* UnityApplications plugin uses GTK APIs to retrieve theme icons
       (gtk_icon_theme_get_default) and requires a call to gtk_init */
    gtk_init(&argc, &argv);

    /* Forcing graphics system to 'raster' instead of the default 'native'
       which on X11 is 'XRender'.
       'XRender' defaults to using a TrueColor visual. We mimick that behaviour
       with 'raster' by calling QApplication::setColorSpec.

       Reference: https://bugs.launchpad.net/upicek/+bug/674484
    */
    QApplication::setGraphicsSystem("raster");
    QApplication::setColorSpec(QApplication::ManyColor);
    QApplication application(argc, argv);

    /* Configure "artwork:" prefix so that any access to a file whose name starts
       with that prefix resolves properly. */
    QDir::addSearchPath("artwork", unityQtDirectory() + "/launcher/artwork");

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

    LauncherControl control;
    view.rootContext()->setContextProperty("launcherControl", &control);
    control.connectToBus();

    view.setSource(QUrl("./Launcher.qml"));

    view.show();
    QDesktopWidget* desktop = QApplication::desktop();
    view.workAreaResized(desktop->screenNumber(&view));
    QObject::connect(desktop, SIGNAL(workAreaResized(int)), &view, SLOT(workAreaResized(int)));

    /* Unset DESKTOP_AUTOSTART_ID in order to avoid child processes (launched
       applications) to use the same client id.
       This would prevent some applications (e.g. nautilus) from launching when
       the launcher itself was autostarted (which is the common case when
       running installed).
       For a discussion, see https://bugs.launchpad.net/upicek/+bug/684160. */
    g_unsetenv("DESKTOP_AUTOSTART_ID");

    return application.exec();
}

