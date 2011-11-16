/*
 * Copyright (C) 2010 Canonical, Ltd.
 *
 * Authors:
 *  Florian Boucault <florian.boucault@canonical.com>
 *  Ugo Riboni <ugo.riboni@canonical.com>
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

// QT
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

// X11
#include <X11/Xlib.h>

// unity-2d
#include <gnomesessionclient.h>
#include <unity2dapplication.h>
#include <unity2ddebug.h>

// Local
#include "config.h"
#include "shelldeclarativeview.h"
#include "dashdbus.h"
#include "gesturehandler.h"
#include "launcherdbus.h"
#include "visibilitycontroller.h"

int main(int argc, char *argv[])
{
    Unity2dApplication::earlySetup(argc, argv);
    Unity2dApplication application(argc, argv);
    application.setApplicationName("Unity 2D Shell");
    QSet<QString> arguments = QSet<QString>::fromList(QCoreApplication::arguments());

    GnomeSessionClient client(INSTALL_PREFIX "/share/applications/unity-2d-shell.desktop");
    client.connectToSessionManager();

    qmlRegisterType<ShellDeclarativeView>("Unity2d", 1, 0, "ShellDeclarativeView");
    ShellDeclarativeView view;
    view.setAccessibleName("Shell");
    if (arguments.contains("-opengl")) {
        view.setUseOpenGL(true);
    }

    DashDBus dashDBus(&view);
    if (!dashDBus.connectToBus()) {
        qCritical() << "Another instance of the Dash already exists. Quitting.";
        return -1;
    }

    // TODO: this should be kept around but not be tied to the panel anymore.
    //    VisibilityController visibilityController(panel);
    //    view.rootContext()->setContextProperty("visibilityController", &visibilityController);

    LauncherDBus launcherDBus(NULL /*&visibilityController*/, &view);
    launcherDBus.connectToBus();

    QDir::addSearchPath("artwork", unity2dDirectory() + "/shell/artwork");

    view.engine()->addImportPath(unity2dImportPath());
    view.engine()->setBaseUrl(QUrl::fromLocalFile(unity2dDirectory() + "/shell/"));

    if (!isRunningInstalled()) {
        /* Allows importing Unity2d when uninstalled */
        view.engine()->addImportPath(unity2dDirectory() + "/libunity-2d-private/");
    }

    /* Load the QML UI, focus and show the window */
    view.setResizeMode(QDeclarativeView::SizeViewToRootObject);
    view.rootContext()->setContextProperty("declarativeView", &view);
    view.setSource(QUrl("qml/Shell.qml"));

    /* Unset DESKTOP_AUTOSTART_ID in order to avoid child processes (launched
       applications) to use the same client id.
       This would prevent some applications (e.g. nautilus) from launching when
       the launcher itself was autostarted (which is the common case when
       running installed).
       For a discussion, see https://bugs.launchpad.net/upicek/+bug/684160. */
    unsetenv("DESKTOP_AUTOSTART_ID");

    /* When spawned via DBus activation, the current working directory is
       inherited from the DBus daemon, and it usually is not the user’s home
       directory. Applications launched from the dash in turn inherit their
       current working directory from the dash, and they expect a sane default
       (see e.g. https://bugs.launchpad.net/bugs/684471). */
    QDir::setCurrent(QDir::homePath());

    return application.exec();
}
