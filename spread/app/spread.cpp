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
#include <QApplication>
#include <QDesktopWidget>
#include <QDeclarativeEngine>
#include <QDeclarativeContext>
#include <QPaintEngine>
#include <QPixmap>

#include "config.h"
#include "spreadview.h"

#include "windowimageprovider.h"

#include <QDebug>
#include <QTimer>
#include <QX11Info>

int main(int argc, char *argv[])
{
    /* UnityApplications plugin uses GTK APIs to retrieve theme icons
       (gtk_icon_theme_get_default) and requires a call to gtk_init */
    gtk_init(&argc, &argv);

    QApplication::setGraphicsSystem("raster");
    QApplication::setColorSpec(QApplication::ManyColor);

    QApplication application(argc, argv);

    SpreadView view;
    application.connect(view.engine(), SIGNAL(quit()), SLOT(quit()));

    view.setAttribute(Qt::WA_X11NetWmWindowTypeDock);
    view.setAttribute(Qt::WA_OpaquePaintEvent);
    view.setAttribute(Qt::WA_NoSystemBackground);
    view.setResizeMode(QDeclarativeView::SizeRootObjectToView);
    view.setFocus();
    view.engine()->addImportPath(unityQtImportPath());

    // This is needed for UnityApplications
    view.engine()->addImportPath(unityQtImportPath() + "/../launcher/");
    view.engine()->setBaseUrl(QUrl::fromLocalFile(unityQtDirectory() + "/spread/"));

    WindowImageProvider *provider = new WindowImageProvider();
    view.engine()->addImageProvider(QString("window"), provider);

    /* Always activate composite, so we can capture windows that are partially obscured
       Ideally we want to activate it only when QX11Info::isCompositingManagerRunning()
       is false, but in my experience it is not reliable at all.
       The only downside when calling this is that there's a small visual glitch at the
       moment when it's called on the entire desktop, and the same thing when the app
       terminates. This happens regardless if the WM has activated composite already or
       not.
    */
    provider->activateComposite();

    view.rootContext()->setContextProperty("spreadView", &view);
    view.rootContext()->setContextProperty("desktop",
                                           QApplication::desktop()->availableGeometry());

    view.setSource(QUrl("./Spread.qml"));
    view.showMaximized();

    return application.exec();
}

