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

// unity-2d
#include <gettexttranslator.h>
#include <gnomesessionclient.h>
#include <unity2dapplication.h>
#include <propertybinder.h>

// libqtgconf
#include <gconfitem-qml-wrapper.h>

// Qt
#include <QApplication>
#include <QDesktopWidget>
#include <QDeclarativeEngine>
#include <QDeclarativeContext>
#include <QDir>
#include <QGraphicsObject>

#include <unity2dapplication.h>

#include "config.h"
#include "launcherview.h"
#include "launcherdbus.h"
#include "visibilitycontroller.h"
#include "unity2dpanel.h"
#include "gesturehandler.h"

int main(int argc, char *argv[])
{
    /* UnityApplications plugin uses GTK APIs to retrieve theme icons
       (gtk_icon_theme_get_default) and requires a call to gtk_init */
    gtk_init(&argc, &argv);

    QApplication::setApplicationName("Unity 2D Launcher");
    qInstallMsgHandler(globalMessageHandler);

    /* Forcing graphics system to 'raster' instead of the default 'native'
       which on X11 is 'XRender'.
       'XRender' defaults to using a TrueColor visual. We mimick that behaviour
       with 'raster' by calling QApplication::setColorSpec.

       Reference: https://bugs.launchpad.net/upicek/+bug/674484
    */
    QApplication::setGraphicsSystem("raster");
    QApplication::setColorSpec(QApplication::ManyColor);
    Unity2dApplication application(argc, argv);

    GnomeSessionClient client(INSTALL_PREFIX "/share/applications/unity-2d-launcher.desktop");
    client.connectToSessionManager();

    /* Configure "artwork:" prefix so that any access to a file whose name starts
       with that prefix resolves properly. */
    QDir::addSearchPath("artwork", unity2dDirectory() + "/launcher/artwork");

    /* Configure translations */
    GettextTranslator translator;
    translator.init("unity-2d", INSTALL_PREFIX "/share/locale");
    QApplication::installTranslator(&translator);

    /* Panel containing the QML declarative view */
    Unity2dPanel panel;
    panel.setEdge(Unity2dPanel::LeftEdge);
    panel.setFixedWidth(66);

    VisibilityController* visibilityController = new VisibilityController(&panel);

    /* QML declarative view */
    LauncherView *launcherView = new LauncherView;

    /* FIXME: possible optimisations */
//    launcherView->setAttribute(Qt::WA_OpaquePaintEvent);
//    launcherView->setAttribute(Qt::WA_NoSystemBackground);
    launcherView->setResizeMode(QDeclarativeView::SizeRootObjectToView);
    launcherView->setFocus();

    launcherView->engine()->addImportPath(unity2dImportPath());
    /* Note: baseUrl seems to be picky: if it does not end with a slash,
       setSource() will fail */
    launcherView->engine()->setBaseUrl(QUrl::fromLocalFile(unity2dDirectory() + "/launcher/"));
    if (!isRunningInstalled()) {
        launcherView->engine()->addImportPath(unity2dDirectory() + "/libunity-2d-private/");
    }

    launcherView->rootContext()->setContextProperty("launcherView", launcherView);
    launcherView->rootContext()->setContextProperty("panel", &panel);
    launcherView->rootContext()->setContextProperty("visibilityController", visibilityController);

    /* FIXME: this is needed since the blended image provider doesn't support relative paths yet */
    launcherView->rootContext()->setContextProperty("engineBaseUrl",
                                                    launcherView->engine()->baseUrl().toLocalFile());

    LauncherDBus control(visibilityController);
    launcherView->rootContext()->setContextProperty("launcherControl", &control);
    control.connectToBus();

    launcherView->setSource(QUrl("./Launcher.qml"));

    /* Synchronise panel's "useStrut" property with its corresponding GConf key */
    GConfItemQmlWrapper useStrutGconf;
    useStrutGconf.setKey("/desktop/unity-2d/launcher/use_strut");
    panel.setUseStrut(useStrutGconf.getValue().toBool());
    PropertyBinder useStrutBinder;
    useStrutBinder.bind(&useStrutGconf, "value", &panel, "useStrut");

    /* Composing the QML declarative view inside the panel */
    panel.addWidget(launcherView);
    panel.show();

    /* Unset DESKTOP_AUTOSTART_ID in order to avoid child processes (launched
       applications) to use the same client id.
       This would prevent some applications (e.g. nautilus) from launching when
       the launcher itself was autostarted (which is the common case when
       running installed).
       For a discussion, see https://bugs.launchpad.net/upicek/+bug/684160. */
    g_unsetenv("DESKTOP_AUTOSTART_ID");

    /* Gesture handler instance in charge of listening to gesture events and
       trigger appropriate actions in response. */
    GestureHandler gestureHandler(&panel);

    return application.exec();
}

