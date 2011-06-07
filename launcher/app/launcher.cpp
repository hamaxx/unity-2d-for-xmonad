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
#include <gnomesessionclient.h>
#include <launcherclient.h>
#include <unity2dapplication.h>
#include <unity2dtr.h>
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

#include "config.h"
#include "launcherview.h"
#include "launcherdbus.h"
#include "visibilitycontroller.h"
#include "unity2ddebug.h"
#include "unity2dpanel.h"
#include "gesturehandler.h"

#if defined(QMLJSDEBUGGER)
#include <qt_private/qdeclarativedebughelper_p.h>
#endif

#if defined(QMLJSDEBUGGER) && !defined(NO_JSDEBUGGER)
#include <jsdebuggeragent.h>
#endif
#if defined(QMLJSDEBUGGER) && !defined(NO_QMLOBSERVER)
#include <qdeclarativeviewobserver.h>
#endif

#if defined(QMLJSDEBUGGER)

// Enable debugging before any QDeclarativeEngine is created
struct QmlJsDebuggingEnabler
{
    QmlJsDebuggingEnabler()
    {
        QDeclarativeDebugHelper::enableDebugging();
    }
};

// Execute code in constructor before first QDeclarativeEngine is instantiated
static QmlJsDebuggingEnabler enableDebuggingHelper;

#endif // QMLJSDEBUGGER

int main(int argc, char *argv[])
{
    /* UnityApplications plugin uses GTK APIs to retrieve theme icons
       (gtk_icon_theme_get_default) and requires a call to gtk_init */
    gtk_init(&argc, &argv);

    Unity2dDebug::installHandlers();

    /* When the environment variable QT_GRAPHICSSYSTEM is not set,
       force graphics system to 'raster' instead of the default 'native'
       which on X11 is 'XRender'.
       'XRender' defaults to using a TrueColor visual. We do _not_ mimick that
       behaviour with 'raster' by calling QApplication::setColorSpec because
       of a bug where black rectangular artifacts were appearing randomly:

       https://bugs.launchpad.net/unity-2d/+bug/734143
    */
    if(getenv("QT_GRAPHICSSYSTEM") == 0) {
        QApplication::setGraphicsSystem("raster");
    }
    Unity2dApplication application(argc, argv);
    QSet<QString> arguments = QSet<QString>::fromList(QCoreApplication::arguments());

    GnomeSessionClient client(INSTALL_PREFIX "/share/applications/unity-2d-launcher.desktop");
    client.connectToSessionManager();

    /* Configure "artwork:" prefix so that any access to a file whose name starts
       with that prefix resolves properly. */
    QDir::addSearchPath("artwork", unity2dDirectory() + "/launcher/artwork");

    /* Configure translations */
    Unity2dTr::init("unity-2d", INSTALL_PREFIX "/share/locale");

    /* Panel containing the QML declarative view */
    Unity2dPanel panel(true);
    panel.setEdge(Unity2dPanel::LeftEdge);
    panel.setFixedWidth(LauncherClient::MaximumWidth);

    VisibilityController* visibilityController = new VisibilityController(&panel);

    /* QML declarative view */
    LauncherView *launcherView = new LauncherView(&panel);
    launcherView->setUseOpenGL(arguments.contains("-opengl"));

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

    Unity2dTr::qmlInit(launcherView->rootContext());

    LauncherDBus launcherDBus(visibilityController, launcherView);
    launcherDBus.connectToBus();

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

