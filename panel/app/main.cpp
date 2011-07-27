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
#include <unity2dstyle.h>
#include <unity2dtr.h>

// Qt
#include <QAbstractFileEngineHandler>
#include <QApplication>
#include <QFSFileEngine>

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
    Unity2dApplication::earlySetup(argc, argv);
    Unity2dApplication app(argc, argv);
    app.setApplicationName("Unity 2D Panel");
    QApplication::setStyle(new Unity2dStyle);

    GnomeSessionClient client(INSTALL_PREFIX "/share/applications/unity-2d-panel.desktop");
    client.connectToSessionManager();

    /* Configure translations */
    Unity2dTr::init("unity-2d", INSTALL_PREFIX "/share/locale");

    PanelManager panels;

    return app.exec();
}
