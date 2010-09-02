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

#include <QApplication>
#include <QDesktopWidget>
#include <QDeclarativeView>
#include <QDeclarativeEngine>

int main(int argc, char *argv[])
{
    QApplication application(argc, argv);

    QDeclarativeView view;
    view.setResizeMode(QDeclarativeView::SizeRootObjectToView);
    view.setFocus();

    if (QCoreApplication::applicationDirPath() == "/usr/bin")
    {
        /* Running installed */
        view.engine()->setBaseUrl(QUrl::fromLocalFile("/usr/share/unity-qt/"));
    }
    else
    {
        /* Uninstalled: make sure local plugins such as UnityApplications are
           importable */
        view.engine()->addImportPath(QString("."));
    }

    /* This is showing the whole unity desktop, not just the launcher: */
    view.setSource(QUrl("./unity_qt.qml"));

    view.resize(QApplication::desktop()->size());
    view.show();

    return application.exec();
}

