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
#include <QX11Info>

#include <X11/Xlib.h>
#include <X11/Xatom.h>

int main(int argc, char *argv[])
{
    QApplication application(argc, argv);

    QDeclarativeView view;
    view.setAttribute(Qt::WA_X11NetWmWindowTypeDesktop);
    /* FIXME: possible optimisations */
//    view.setAttribute(Qt::WA_OpaquePaintEvent);
//    view.setAttribute(Qt::WA_NoSystemBackground);
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
    view.setSource(QUrl("./Launcher.qml"));

    view.resize(60, QApplication::desktop()->size().height());
    view.show();

    /* Reserve space at the left edge of the screen (the launcher is a panel) */
    Atom atom = XInternAtom(QX11Info::display(), "_NET_WM_STRUT_PARTIAL", False);
    uint struts[12] = {view.size().width(), 0, 0, 0, 0, view.size().height(),
                       0, 0, 0, 0, 0, 0};
    XChangeProperty(QX11Info::display(), view.effectiveWinId(), atom,
                    XA_CARDINAL, 32, PropModeReplace,
                    (unsigned char *) &struts, 12);

    return application.exec();
}

