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

#include "config.h"

int main(int argc, char *argv[])
{
    QApplication application(argc, argv);

    QDeclarativeView view;
    view.setResizeMode(QDeclarativeView::SizeRootObjectToView);
    view.setFocus();

    /* Performance tricks */
    view.setAttribute(Qt::WA_OpaquePaintEvent);
    view.setAttribute(Qt::WA_NoSystemBackground);
    //QApplication::setGraphicsSystem("raster");

    if (QCoreApplication::applicationDirPath() == INSTALL_PREFIX "/bin")
    {
        /* Running installed */
        /* Note: baseUrl seems to be picky: if it does not end with a slash,
           setSource() will fail */
        view.engine()->setBaseUrl(QUrl::fromLocalFile(INSTALL_PREFIX "/" UNITY_QT_DIR "/"));
    }
    else
    {
        /* Uninstalled: make sure local plugins such as QtDee are
           importable */
        view.engine()->addImportPath(QString("."));
    }

    view.setSource(QUrl("./places.qml"));
    view.show();

    return application.exec();
}
