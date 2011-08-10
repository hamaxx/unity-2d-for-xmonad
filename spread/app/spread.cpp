/*
 * Copyright (C) 2010 Canonical, Ltd.
 *
 * Authors:
 *  Ugo Riboni <ugo.riboni@canonical.com>
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
#include <QDesktopWidget>
#include <QDeclarativeEngine>
#include <QDeclarativeContext>

#include "spreadview.h"
#include "spreadcontrol.h"
#include "launcherclient.h"

#include <unity2dapplication.h>

#include "config.h"

int main(int argc, char *argv[])
{
    Unity2dApplication::earlySetup(argc, argv);
    Unity2dApplication application(argc, argv);
    application.setApplicationName("Unity 2D Workspace Switcher");
    QSet<QString> arguments = QSet<QString>::fromList(QCoreApplication::arguments());

    SpreadView view;
    view.setUseOpenGL(arguments.contains("-opengl"));

    /* The spread window is borderless and not moveable by the user, yet not
       fullscreen */
    view.setAttribute(Qt::WA_X11NetWmWindowTypeDock, true);

    view.engine()->addImportPath(unity2dImportPath());
    /* Note: baseUrl seems to be picky: if it does not end with a slash,
       setSource() will fail */
    view.engine()->setBaseUrl(QUrl::fromLocalFile(unity2dDirectory() + "/spread/"));

    if (!isRunningInstalled()) {
        /* Spread.qml imports Unity2d */
        view.engine()->addImportPath(unity2dDirectory() + "/libunity-2d-private/");
    }

    /* Add a SpreadControl instance to the QML context */
    /* FIXME: the SpreadControl class should be exposed to QML by a plugin and
              instantiated on the QML side */
    SpreadControl control;
    control.connectToBus();
    control.connect(&view, SIGNAL(visibleChanged(bool)), SLOT(setIsShown(bool)));
    view.rootContext()->setContextProperty("control", &control);

    /* Load the QML UI, focus and show the window */
    view.setResizeMode(QDeclarativeView::SizeRootObjectToView);
    view.rootContext()->setContextProperty("declarativeView", &view);
    view.rootContext()->setContextProperty("spreadView", &view);
    view.setSource(QUrl("./Workspaces.qml"));

    /* Always match the size of the desktop */
    int current_screen = QApplication::desktop()->screenNumber(&view);
    view.fitToAvailableSpace(current_screen);
    QObject::connect(QApplication::desktop(), SIGNAL(workAreaResized(int)), &view, SLOT(fitToAvailableSpace(int)));

    return application.exec();
}
