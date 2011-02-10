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
#include <QDeclarativeEngine>
#include <QDeclarativeView>
#include <QDesktopWidget>
#include <QDBusConnection>
#include <QDBusConnectionInterface>
#include <QDeclarativeContext>
#include <QAbstractEventDispatcher>

#include <X11/Xlib.h>

#include "dashdeclarativeview.h"
#include "superkeymonitor.h"

#include "config.h"

/* Register a D-Bus service for activation and deactivation of the dash */
static bool registerDBusService(DashDeclarativeView* view)
{
    QDBusConnection bus = QDBusConnection::sessionBus();
    if (!bus.registerService("com.canonical.Unity2d")) {
        qCritical() << "Failed to register DBus service, is there another instance already running?";
        return false;
    }
    /* FIXME: use an adaptor class in order not to expose all of the view's
       properties and methods. */
    if (!bus.registerObject("/Dash", view, QDBusConnection::ExportAllContents)) {
        qCritical() << "Failed to register /Dash, this should not happen!";
        return false;
    }
    /* It would be nice to support the newly introduced (D-Bus 0.14 07/09/2010)
       property change notification that Qt 4.7 does not implement.

        org.freedesktop.DBus.Properties.PropertiesChanged (
            STRING interface_name,
            DICT<STRING,VARIANT> changed_properties,
            ARRAY<STRING> invalidated_properties);

       ref.: http://randomguy3.wordpress.com/2010/09/07/the-magic-of-qtdbus-and-the-propertychanged-signal/
    */
    return true;
}

static DashDeclarativeView* getView()
{
    QVariant viewProperty = QApplication::instance()->property("view");
    return viewProperty.value<DashDeclarativeView*>();
}

static bool eventFilter(void* message)
{
    XEvent* event = static_cast<XEvent*>(message);
    if (event->type == KeyRelease)
    {
        XKeyEvent* key = (XKeyEvent*) event;
        uint code = key->keycode;
        if (code == SuperKeyMonitor::SUPER_L || code == SuperKeyMonitor::SUPER_R) {
            /* Super (aka the "windows" key) shows/hides the dash. */
            DashDeclarativeView* view = getView();
            if (view->active()) {
                view->setActive(false);
            }
            else {
                view->activateHome();
            }
        }
    }
    return false;
}

int main(int argc, char *argv[])
{
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

    DashDeclarativeView view;

    if (!registerDBusService(&view)) {
        return -1;
    }

    /* The dash window is borderless and not moveable by the user, yet not
       fullscreen */
    view.setAttribute(Qt::WA_X11NetWmWindowTypeDock, true);

    /* Performance tricks */
    view.setAttribute(Qt::WA_OpaquePaintEvent);
    view.setAttribute(Qt::WA_NoSystemBackground);

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

    /* Always match the size of the desktop */
    int current_screen = QApplication::desktop()->screenNumber(&view);
    view.fitToAvailableSpace(current_screen);
    QObject::connect(QApplication::desktop(), SIGNAL(workAreaResized(int)), &view, SLOT(fitToAvailableSpace(int)));

    /* Grab the "super" keys */
    SuperKeyMonitor superKeys; /* Just needs to be instantiated to work. */
    QAbstractEventDispatcher::instance()->setEventFilter(eventFilter);

    application.setProperty("view", QVariant::fromValue(&view));
    return application.exec();
}
