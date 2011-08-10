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
#include <QPixmap>
#include <QPainter>
#include <QImage>

#include "windowimageprovider.h"
#include <debug_p.h>

#include <X11/Xlib.h>
#include <X11/Xatom.h>
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

/* Static helper to read the _METACITY_WINDOW_CAPTURE window property
   which contains the captured pixmap for unmapped windows.
   This property is created only when running our patched version of
   metacity that can be found in: lp:~unity-2d-team/unity-2d/metacity
   If it does exist it contains a pixmap with a screenshot of the window
   before it was unmapped (due to moving to another workspace or being
   minimized).

   NOTE: XID should really be Pixmap (from Xlib). However the definition
   clashes with QImage's Pixmap, so I'm forced to use just XID. It's the
   same underlying type anyway, so it doesn't cause issues.
*/
static bool tryGetWindowCapture(Window xwindow, XID *pixmap)
{
  /* Intern the atom only once, as its value never changes */
  static Atom atom = XInternAtom(QX11Info::display(),
                                 "_METACITY_WINDOW_CAPTURE", False);
  Atom type;
  int format;
  unsigned long nitems;
  unsigned long bytes_after;
  Pixmap *data;
  int result;

  *pixmap = 0;

  type = None;
  result = XGetWindowProperty (QX11Info::display(),
                               xwindow, atom,
                               0, LONG_MAX,
                               False, XA_PIXMAP, &type, &format, &nitems,
                               &bytes_after, (unsigned char**)&data);
  if (result != Success) {
     return false;
  }

  if (type != XA_PIXMAP) {
      XFree (data);
      return false;
  }

  *pixmap = *data;
  XFree (data);
  return true;
}

QImage WindowImageProvider::requestImage(const QString &id,
                                              QSize *size,
                                              const QSize &requestedSize)
{
    /* Throw away the part of the id after the @ (if any) since it's just a timestamp
       added to force the QML image cache to request to this image provider
       a new image instead of re-using the old. See SpreadWindow.qml for more
       details on the problem. */
    int atPos = id.indexOf('@');
    QString windowIds = (atPos == -1) ? id : id.left(atPos);

    /* After doing this, split the rest of the id on the character "|". The first
       part is the window ID of the decorations, the latter of the actual content. */
    atPos = windowIds.indexOf('|');
    Window frameId = ((atPos == -1) ? windowIds : windowIds.left(atPos)).toULong();
    Window contentId = ((atPos == -1) ? windowIds : windowIds.mid(atPos + 1)).toULong();

    /* Use form image://window/root to specify you want an image of the root window */
    if (atPos == -1 && windowIds == "root") {
        frameId = QX11Info::appRootWindow();
    }

    QImage image;
    QPixmap pixmap = getWindowPixmap(frameId, contentId);
    if (!pixmap.isNull()) {
        image = convertWindowPixmap(pixmap, frameId);
        if (image.isNull()) {
            /* This means that the window got unmapped while we were converting it to
               an image. Try again so that we will pick up the pixmap captured by
               metacity instead. */
            pixmap = getWindowPixmap(frameId, contentId);
            if (!pixmap.isNull()) {
                image = convertWindowPixmap(pixmap, frameId);
            }
        }
    }

    if (!image.isNull()) {
        if (requestedSize.isValid()) {
            image = image.scaled(requestedSize);
        }
        size->setWidth(image.width());
        size->setHeight(image.height());
    }

    return image;
}

QPixmap WindowImageProvider::getWindowPixmap(Window frameWindowId,
                                             Window contentWindowId)
{
    XWindowAttributes attr;

    XGetWindowAttributes(QX11Info::display(), frameWindowId, &attr);
    if (attr.map_state == IsViewable) {
        return QPixmap::fromX11Pixmap(frameWindowId);
    } else {
        /* If the window is not viewable, grabbing the pixmap directly will fail.
           Therefore we try to retrieve the captured pixmap from the special metacity
           window property (if present).
           NOTE: even if stored on the content window, this pixmap contains the entire
           window including decorations.
        */
        XID pixmap;
        if (!tryGetWindowCapture(contentWindowId, &pixmap)) {
            return QPixmap();
        } else {
            return QPixmap::fromX11Pixmap(pixmap);
        }
    }
}

/* All the exception checks in this method are due to the fact that
   the windowPixmap is an X11 Drawable tied to a window. When we called
   this function the drawable was valid since the window was mapped, however
   there's no guarantee it will stay that way.
   All the operations we do here involve at some point converting the pixmap
   into a QImage (since we use the rester paint engine). This conversion will
   throw std::bad_alloc if the drawable is not valid, so we need to catch it.
*/
QImage WindowImageProvider::convertWindowPixmap(QPixmap windowPixmap,
                                                Window frameWindowId)
{
    if (m_x11supportsShape) {
        /* The borders of the window may be irregularly shaped.
           To capture this correctly we need to ask the Shape extension
           to decompose the non-transparent window are into rectangles,
           then copy each of them from the screenshot to our
           destination image (which is entirely transparent initally) */
        XRectangle *rectangles;
        int rectangle_count, rectangle_order;
        rectangles = XShapeGetRectangles (QX11Info::display(),
                                          frameWindowId,
                                          ShapeBounding,
                                          &rectangle_count,
                                          &rectangle_order);

        QImage result(windowPixmap.size(), QImage::Format_ARGB32_Premultiplied);
        result.fill(Qt::transparent);
        QPainter painter(&result);

        for (int i = 0; i < rectangle_count; i++) {
            XRectangle r = rectangles[i];
            try {
                /* Since we're using the raster paint engine, this will internally
                   call QX11PixmapData::toImage on windowPixmap to be able to
                   draw it on the QImage */
                painter.drawPixmap(QPointF(r.x, r.y), windowPixmap,
                                   QRectF(r.x, r.y, r.width, r.height));
            } catch (std::bad_alloc) {
                return QImage();
            }
        }

        painter.end();
        XFree(rectangles);
        return result;
    } else {
        try {
            return windowPixmap.toImage();
        } catch (std::bad_alloc) {
            return QImage();
        }
    }
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
            (UQ_DEBUG).nospace() << "Server supports the Composite extension (ver "
                    << major << "." << minor << ")";
        }
        else {
            (UQ_DEBUG).nospace() << "Server supports the Composite extension, but "
                                  "version is < 0.2 (ver " << major << "." << minor << ")";
        }
    } else {
        UQ_DEBUG << "Server doesn't support the Composite extension.";
    }

    if (compositeSupport) {
        int screens = ScreenCount(display);
        for (int i = 0; i < screens; ++i) {
            XCompositeRedirectSubwindows(display, RootWindow(display, i),
                                         CompositeRedirectAutomatic);
        }
    }
}
