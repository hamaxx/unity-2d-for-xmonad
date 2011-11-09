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

// unity-2d
#include <unity2dapplication.h>
#include <unity2ddebug.h>

#include "dashdeclarativeview.h"
#include "config.h"

int main(int argc, char *argv[])
{
    Unity2dApplication::earlySetup(argc, argv);
    Unity2dApplication application(argc, argv);
    application.setApplicationName("Unity 2D Dash");
    QSet<QString> arguments = QSet<QString>::fromList(QCoreApplication::arguments());

    qmlRegisterType<DashDeclarativeView>("Unity2d", 1, 0, "DashDeclarativeView");
    DashDeclarativeView view;
    view.setAccessibleName("Dash");
    view.setUseOpenGL(arguments.contains("-opengl"));

    if (!view.connectToBus()) {
        qCritical() << "Another instance of the Dash already exists. Quitting.";
        return -1;
    }

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
    view.rootContext()->setContextProperty("declarativeView", &view);
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
