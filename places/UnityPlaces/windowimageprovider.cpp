/*
 * Copyright (C) 2010 Canonical, Ltd.
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

#include <QX11Info>
#include <QDebug>
#include <QPixmap>
#include <QPainter>
#include <QImage>

#include "windowimageprovider.h"

#include <X11/Xlib.h>
#include <X11/extensions/Xcomposite.h>
#include <X11/extensions/shape.h>

WindowImageProvider::WindowImageProvider() :
    QDeclarativeImageProvider(QDeclarativeImageProvider::Image), m_x11supportsShape(false)
{
    /* Always activate composite, so we can capture windows that are partially obscured
       Ideally we want to activate it only when QX11Info::isCompositingManagerRunning()
       is false, but in my experience it is not reliable at all.
       The only downside when calling this is that there's a small visual glitch at the
       moment when it's called on the entire desktop, and the same thing when the app
       terminates. This happens regardless if the WM has activated composite already or
       not.
    */
    activateComposite();

    int event_base, error_base;
    m_x11supportsShape = XShapeQueryExtension(QX11Info::display(),
                                              &event_base, &error_base);
}

WindowImageProvider::~WindowImageProvider()
{
}

QImage WindowImageProvider::requestImage(const QString &id,
                                              QSize *size,
                                              const QSize &requestedSize)
{
    /* Throw away the part of the id after the @ (if any) since it's just a timestamp
       added to force the QML image cache to request to this image provider
       a new image instead of re-using the old. See SpreadWindow.qml for more
       details on the problem */
    int atPos = id.indexOf('@');
    QString windowId = (atPos == -1) ? id : id.left(atPos);

    Window win = (Window) windowId.toULong();
    XWindowAttributes attr;
    XGetWindowAttributes(QX11Info::display(), win, &attr);
    if (attr.map_state != IsViewable) {
        return QImage();
    }

    QPixmap shot = QPixmap::fromX11Pixmap(win);
    if (shot.isNull()) {
        return QImage();
    }

    QImage result;
    if (m_x11supportsShape) {
        /* The borders of the window may be irregularly shaped.
           To capture this correctly we need to ask the Shape extension
           to decompose the non-transparent window are into rectangles,
           then copy each of them from the screenshot to our
           destination image (which is entirely transparent initally) */
        XRectangle *rectangles;
        int rectangle_count, rectangle_order;
        rectangles = XShapeGetRectangles (QX11Info::display(),
                                          win,
                                          ShapeBounding,
                                          &rectangle_count,
                                          &rectangle_order);

        result = QImage(shot.size(), QImage::Format_ARGB32_Premultiplied);
        result.fill(Qt::transparent);
        QPainter painter(&result);

        for (int i = 0; i < rectangle_count; i++) {
            XRectangle r = rectangles[i];
            painter.drawPixmap(QPointF(r.x, r.y), shot,
                               QRectF(r.x, r.y, r.width, r.height));
        }

        painter.end();
        XFree(rectangles);
    } else {
        result = shot.toImage();
    }

    if (requestedSize.isValid()) {
        result = result.scaled(requestedSize);
    }
    size->setWidth(result.width());
    size->setHeight(result.height());

    return result;
}

/*! Tries to ask the X Composite extension (if supported) to redirect all
    windows on all screens to backing storage. This does not have
    any effect if another application already requested the same
    thing (typically the window manager does this).
*/
void WindowImageProvider::activateComposite()
{
    int event_base;
    int error_base;

    Display *display = QX11Info::display();
    bool compositeSupport = false;

    if (XCompositeQueryExtension(display, &event_base, &error_base)) {
        int major = 0;
        int minor = 2;
        XCompositeQueryVersion(display, &major, &minor);

        if (major > 0 || minor >= 2) {
            compositeSupport = true;
            qDebug().nospace() << "Server supports the Composite extension (ver "
                    << major << "." << minor << ")";
        }
        else {
            qDebug().nospace() << "Server supports the Composite extension, but "
                                  "version is < 0.2 (ver " << major << "." << minor << ")";
        }
    } else {
        qDebug() << "Server doesn't support the Composite extension.";
    }

    if (compositeSupport) {
        int screens = ScreenCount(display);
        for (int i = 0; i < screens; ++i) {
            XCompositeRedirectSubwindows(display, RootWindow(display, i),
                                         CompositeRedirectAutomatic);
        }
    }
}
