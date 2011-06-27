/*
 * This file is part of unity-2d
 *
 * Copyright 2010 Canonical Ltd.
 *
 * Authors:
 * - Aurélien Gâteau <aurelien.gateau@canonical.com>
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

// Local
#include <config.h>
#include <panelmanager.h>

// Unity
#include <gnomesessionclient.h>
#include <unity2ddebug.h>
#include <unity2dapplication.h>
#include <unity2dtr.h>

// Qt
#include <QAbstractFileEngineHandler>
#include <QApplication>
#include <QFSFileEngine>

// GTK
#include <gtk/gtk.h>

class ThemeEngineHandler : public QAbstractFileEngineHandler
{
public:
    QAbstractFileEngine *create(const QString& fileName) const
    {
        if (fileName.startsWith("theme:")) {
            QString name = UNITY_DIR "themes/" + fileName.mid(6);
            return new QFSFileEngine(name);
        } else {
            return 0;
        }
    }
};

int main(int argc, char** argv)
{
    ThemeEngineHandler handler;

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
    Unity2dApplication app(argc, argv);
    gtk_init(&argc, &argv);

    GnomeSessionClient client(INSTALL_PREFIX "/share/applications/unity-2d-panel.desktop");
    client.connectToSessionManager();

    /* Configure translations */
    Unity2dTr::init("unity-2d", INSTALL_PREFIX "/share/locale");

    PanelManager panels;

    return app.exec();
}
