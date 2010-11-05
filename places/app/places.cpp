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
    /* FIXME: this should not be needed but some parts of the dash are not
              property refreshed when not using it .. sometimes */
    view.setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
    QApplication::setGraphicsSystem("raster");

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
    view.rootContext()->setContextProperty("engineBaseUrl", view.engine()->baseUrl().toLocalFile());
    view.setSource(QUrl("./dash.qml"));

    /* Always match the size of the desktop */
    int current_screen = QApplication::desktop()->screenNumber(&view);
    view.fitToAvailableSpace(current_screen);
    QObject::connect(QApplication::desktop(), SIGNAL(workAreaResized(int)), &view, SLOT(fitToAvailableSpace(int)));

    /* Register a D-Bus service for activation and deactivation of the dash */
    QDBusConnection bus = QDBusConnection::sessionBus();
    bus.registerService("com.canonical.UnityQt");
    /* FIXME: use an adaptor class in order not to expose all of the view's
       properties and methods. */
    bus.registerObject("/dash", &view, QDBusConnection::ExportAllContents);
    /* It would be nice to support the newly introduced (D-Bus 0.14 07/09/2010)
       property change notification that Qt 4.7 does not implement.

        org.freedesktop.DBus.Properties.PropertiesChanged (
            STRING interface_name,
            DICT<STRING,VARIANT> changed_properties,
            ARRAY<STRING> invalidated_properties);

       ref.: http://randomguy3.wordpress.com/2010/09/07/the-magic-of-qtdbus-and-the-propertychanged-signal/
    */

    view.setActive(false);
    view.show();

    return application.exec();
}
