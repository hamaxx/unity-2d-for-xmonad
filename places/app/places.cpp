/*
 * Copyright (C) 2010 Canonical, Ltd.
 *
 * Authors:
 *  Florian Boucault <florian.boucault@canonical.com>
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

#include <QApplication>
#include <QDebug>
#include <QtDeclarative>
#include <QDeclarativeEngine>
#include <QDeclarativeView>
#include <QDesktopWidget>
#include <QDBusConnection>
#include <QDBusConnectionInterface>
#include <QDeclarativeContext>
#include <QAbstractEventDispatcher>
#include <QDir>

#include <X11/Xlib.h>

#undef signals
#include <gtk/gtk.h>

// unity-2d
#include <unity2ddebug.h>
#include <unity2dtr.h>

#include "dashdeclarativeview.h"
#include "config.h"

int main(int argc, char *argv[])
{
    /* gtk needs to be inited, otherwise we get an assert failure in gdk */
    gtk_init(&argc, &argv);
    Unity2dDebug::installHandlers();

    /* When the environment variable QT_GRAPHICSSYSTEM is not set,
       force graphics system to 'raster' instead of the default 'native'
       which on X11 is 'XRender'.
       'XRender' defaults to using a TrueColor visual. We do _not_ mimick that
       behaviour with 'raster' by calling QApplication::setColorSpec because
       of a bug where some pixmaps become blueish:

       https://bugs.launchpad.net/unity-2d/+bug/689877
    */
    if(getenv("QT_GRAPHICSSYSTEM") == 0) {
        QApplication::setGraphicsSystem("raster");
    }
    QApplication application(argc, argv);
    QSet<QString> arguments = QSet<QString>::fromList(QCoreApplication::arguments());

    qmlRegisterType<DashDeclarativeView>("Places", 1, 0, "DashDeclarativeView");
    DashDeclarativeView view;
    view.setUseOpenGL(arguments.contains("-opengl"));

    if (!view.connectToBus()) {
        qCritical() << "Another instance of the Dash already exists. Quitting.";
        return -1;
    }

    /* Configure translations */
    Unity2dTr::init("unity-2d", INSTALL_PREFIX "/share/locale");
    Unity2dTr::qmlInit(view.rootContext());

    view.engine()->addImportPath(unity2dImportPath());
    /* Note: baseUrl seems to be picky: if it does not end with a slash,
       setSource() will fail */
    view.engine()->setBaseUrl(QUrl::fromLocalFile(unity2dDirectory() + "/places/"));

    if (!isRunningInstalled()) {
        /* Place.qml imports Unity2d */
        view.engine()->addImportPath(unity2dDirectory() + "/libunity-2d-private/");
    }

    /* Load the QML UI, focus and show the window */
    view.setResizeMode(QDeclarativeView::SizeRootObjectToView);
    view.rootContext()->setContextProperty("dashView", &view);
    view.setSource(QUrl("./dash.qml"));

    /* When spawned via DBus activation, the current working directory is
       inherited from the DBus daemon, and it usually is not the user’s home
       directory. Applications launched from the dash in turn inherit their
       current working directory from the dash, and they expect a sane default
       (see e.g. https://bugs.launchpad.net/bugs/684471). */
    QDir::setCurrent(QDir::homePath());

    application.setProperty("view", QVariant::fromValue(&view));
    return application.exec();
}
