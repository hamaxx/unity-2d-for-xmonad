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

#include <X11/Xlib.h>

#undef signals
#include <gtk/gtk.h>

// unity-2d
#include <gettexttranslator.h>

#include "dashdeclarativeview.h"
#include "config.h"

int main(int argc, char *argv[])
{
    /* gtk needs to be inited, otherwise we get an assert failure in gdk */
    gtk_init(&argc, &argv);
    QApplication::setApplicationName("Unity 2D Dash");
    qInstallMsgHandler(globalMessageHandler);

    /* Forcing graphics system to 'raster' instead of the default 'native'
       which on X11 is 'XRender'.
       'XRender' defaults to using a TrueColor visual. We do _not_ mimick that
       behaviour with 'raster' by calling QApplication::setColorSpec because
       of a bug where some pixmaps become blueish:

       https://bugs.launchpad.net/unity-2d/+bug/689877
    */
    QApplication::setGraphicsSystem("raster");
    QApplication application(argc, argv);

    qmlRegisterType<DashDeclarativeView>("Places", 1, 0, "DashDeclarativeView");
    DashDeclarativeView view;
    if (!view.connectToBus()) {
        qCritical() << "Another instance of the Dash already exists. Quitting.";
        return -1;
    }

    /* Configure translations */
    GettextTranslator translator;
    translator.init("unity-2d", INSTALL_PREFIX "/share/locale");
    QApplication::installTranslator(&translator);

    view.engine()->addImportPath(unity2dImportPath());
    /* Note: baseUrl seems to be picky: if it does not end with a slash,
       setSource() will fail */
    view.engine()->setBaseUrl(QUrl::fromLocalFile(unity2dDirectory() + "/places/"));

    if (!isRunningInstalled()) {
        /* Place.qml imports UnityApplications, which is part of the launcher
           component… */
        view.engine()->addImportPath(unity2dDirectory() + "/launcher/");
        /* Place.qml imports Unity2d */
        view.engine()->addImportPath(unity2dDirectory() + "/libunity-2d-private/");
    }

    /* Load the QML UI, focus and show the window */
    view.setResizeMode(QDeclarativeView::SizeRootObjectToView);
    view.rootContext()->setContextProperty("dashView", &view);
    view.rootContext()->setContextProperty("engineBaseUrl", view.engine()->baseUrl().toLocalFile());
    view.setSource(QUrl("./dash.qml"));

    application.setProperty("view", QVariant::fromValue(&view));
    return application.exec();
}
