/*
 * Copyright (C) 2010 Canonical, Ltd.
 *
 * Authors:
 *  Florian Boucault <florian.boucault@canonical.com>
 *  Ugo Riboni <ugo.riboni@canonical.com>
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

// QT
#include <QApplication>
#include <QX11Info>
#include <QDebug>
#include <QStringList>

#include <QBitmap>
#include <QDesktopWidget>
#include <QFile>
#include <QImageWriter>
#include <QRegion>
#include <QPainter>
#include <QPainterPath>

// X11
#include <X11/Xlib.h>
#include <X11/extensions/shape.h>

void printUsage()
{
    qDebug() << "ARGUMENTS: window_id [output_file]";
}

int handle_x11_errors(Display *display, XErrorEvent *theEvent)
{
    qWarning() << "Failed to call X11 function XShapeGetRectangles.";
    qWarning() << "The window ID that was passed to it was probably not valid anymore.";
    qWarning() << "X error code:" << theEvent->error_code;
    qWarning() << "X Request code" << theEvent->request_code;
    exit(4);
}

int main(int argc, char *argv[])
{
    QApplication application(argc, argv);
    QStringList arguments = application.arguments();

    if (arguments.length() < 2) {
        printUsage();
        return 1;
    }

    bool ok;
    Window windowId = arguments.at(1).toLongLong(&ok, 0);
    if (!ok) {
        qWarning() << "Window ID is not a valid integer:" << arguments.at(1);
        return 2;
    }

    QString outputFile;
    if (arguments.length() > 2) {
        outputFile = arguments.at(2);
    }

    XErrorHandler old_handler = XSetErrorHandler(handle_x11_errors);

    QRegion region;
    int count, ordering;
    XRectangle *rects = XShapeGetRectangles(QX11Info::display(), windowId,
                                            ShapeInput, &count, &ordering);
    XSync(QX11Info::display(), False);
    XSetErrorHandler(old_handler);

    for (int i = 0; i < count; i++) {
        region = region.united(QRect(rects[i].x, rects[i].y, rects[i].width, rects[i].height));
    }

    if (!outputFile.isEmpty()) {
        Atom actual_type;
        int actual_format;
        unsigned long nitems, bytes_after;
        unsigned char *data;
        const Atom atom = XInternAtom(QX11Info::display(), "_NET_WM_DESKTOP", False);
        const int status = XGetWindowProperty(QX11Info::display(), windowId, atom, 0, (~0L),
                                              False, AnyPropertyType, &actual_type,
                                              &actual_format, &nitems, &bytes_after,
                                              &data);
        int desktop = -1;
        if (status == Success) {
            if (nitems == 1) {
                desktop = *((int*)data);
            }
            free(data);
        }
  
        const QDesktopWidget dw;
        QBitmap bitmap(dw.availableGeometry(desktop).size());
        bitmap.fill(Qt::color0);
        QPainter painter(&bitmap);
        QPainterPath path;
        path.addRegion(region);
        painter.fillPath(path, Qt::color1);
        painter.end();

        if (outputFile == "-") {
            QFile f;
            if (!f.open(stdout, QIODevice::WriteOnly)) {
                return 4;
            }
            QImageWriter writer(&f, "PNG");
            if (!writer.write(bitmap.toImage())) {
                return 5;
            }
        } else if (!bitmap.save(outputFile)) {
            qWarning() << "Failed to save file with path:" << outputFile;
            return 3;
        }
    }

    return 0;
}
