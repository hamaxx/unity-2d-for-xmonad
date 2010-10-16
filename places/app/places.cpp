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
#include <QDeclarativeEngine>
#include <QDeclarativeView>
#include <QDesktopWidget>
#include <QDBusConnection>
#include <QDBusConnectionInterface>
#include <QDeclarativeContext>

#include "dashdeclarativeview.h"

#include "config.h"

int main(int argc, char *argv[])
{
    QApplication application(argc, argv);

    DashDeclarativeView view;

    /* Performance tricks */
    view.setAttribute(Qt::WA_OpaquePaintEvent);
    view.setAttribute(Qt::WA_NoSystemBackground);
    //QApplication::setGraphicsSystem("raster");

    if (QCoreApplication::applicationDirPath() == INSTALL_PREFIX "/bin")
    {
        /* Running installed */
        view.engine()->addImportPath(QString(INSTALL_PREFIX "/lib/qt4/imports"));
        /* Note: baseUrl seems to be picky: if it does not end with a slash,
           setSource() will fail */
        view.engine()->setBaseUrl(QUrl::fromLocalFile(INSTALL_PREFIX "/" UNITY_QT_DIR "/places/"));
    }
    else
    {
        /* Uninstalled: make sure local plugins such as QtDee are
           importable */
        view.engine()->addImportPath(QString("."));
        /* Place.qml imports UnityApplications, which is part of the launcher
           component… */
        view.engine()->addImportPath(QString("../launcher"));
    }

    /* Load the QML UI, focus and show the window */
    view.setResizeMode(QDeclarativeView::SizeRootObjectToView);
    view.rootContext()->setContextProperty("dashView", &view);
    view.setSource(QUrl("./dash.qml"));

    /* Always match the size of the desktop */
    int current_screen = QApplication::desktop()->screenNumber(&view);
    view.fitToAvailableSpace(current_screen);
    QObject::connect(QApplication::desktop(), SIGNAL(workAreaResized(int)), &view, SLOT(fitToAvailableSpace(int)));

    /* Register a D-Bus service for activation and deactivation of the dash */
    QDBusConnection bus = QDBusConnection::sessionBus();
    bus.registerService("com.canonical.UnityQt");
    bus.registerObject("/dash", &view, QDBusConnection::ExportAllProperties);

    view.setActive(false);
    view.show();

    return application.exec();
}
