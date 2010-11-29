#include <QX11Info>
#include <QDebug>
#include <QPaintEngine>
#include <QImage>

#include "windowgrabber.h"

#include <X11/Xlib.h>
#include <X11/extensions/Xcomposite.h>
#include <X11/extensions/Xrender.h>

WindowGrabber::WindowGrabber(QObject *parent, bool useRender) :
        QObject(parent), m_display(0), m_outputPath(QString()),
        m_compositeSupport(false), m_renderSupport(false), m_useRender(useRender) {

    int event_base;
    int error_base;

    m_display = QX11Info::display();

    if (XCompositeQueryExtension(m_display, &event_base, &error_base)) {
        int major = 0;
        int minor = 2;
        XCompositeQueryVersion(m_display, &major, &minor);

        if (major > 0 || minor >= 2) {
            m_compositeSupport = true;
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

    if (m_compositeSupport) {
        int screens = ScreenCount(m_display);
        for (int i = 0; i < screens; ++i) {
            XCompositeRedirectSubwindows(m_display, RootWindow(m_display, i),
                                         CompositeRedirectAutomatic);
        }
    }

    int render_base_event;
    int render_base_error;
    m_renderSupport = XRenderQueryExtension(m_display,
                                            &render_base_event,
                                            &render_base_error);
    if (m_renderSupport) {
        qDebug() << "Server supports the Render extension.";
    } else {
        qDebug() << "Server doesn't support the Render extension.";
    }
}

WindowGrabber::~WindowGrabber() {
}

QString WindowGrabber::outputPath() const {
    return m_outputPath;
}

void WindowGrabber::setOutputPath(const QString path) {
    m_outputPath = path;
}

QImage* WindowGrabber::getImageForWindow(Window windowId) {
    XWindowAttributes attr;
    // TODO: figure out which of the informations restrieved from XGetWindowAttributes
    // and XRenderFindVisualFormat can be cached, and perhaps invalidated only by
    // listening from specific X11 events.
    XGetWindowAttributes(m_display, windowId, &attr);

    // If we're using the raster or OpenGL graphics systems, a QPixmap isn't an X pixmap,
    // so we have to get the window contents into a QImage
    Pixmap xpixmap = XCompositeNameWindowPixmap(m_display, windowId);
    XImage *ximage = XGetImage(m_display, xpixmap, 0, 0, attr.width, attr.height, AllPlanes, ZPixmap);
    XFreePixmap(m_display, xpixmap);

    if (!ximage) {
        return 0;
    }

    // This is safe to do since we only composite ARGB32 windows, and PictStandardARGB32
    // matches QImage::Format_ARGB32_Premultiplied.
    QImage *image = new QImage((const uchar*)ximage->data, ximage->width, ximage->height,
                              ximage->bytes_per_line, QImage::Format_ARGB32_Premultiplied);
    XDestroyImage(ximage);

    return image;
}

QPixmap* WindowGrabber::getPixmapForWindow(Window windowId) {

    return new QPixmap(QPixmap::fromX11Pixmap(windowId));

//    XWindowAttributes attr;
//    XRenderPictFormat *format;
//    XRenderPictureAttributes pa;
//    Picture picture;

//    int x;
//    int y;
//    int width ;
//    int height ;
//    QPixmap *pixmap;

//    if (!m_compositeSupport) {
//        return 0;
//    }

//    // TODO: figure out which of the informations restrieved from XGetWindowAttributes
//    // and XRenderFindVisualFormat can be cached, and perhaps invalidated only by
//    // listening from specific X11 events.
//    XGetWindowAttributes(m_display, windowId, &attr);
//    x         = attr.x;
//    y         = attr.y;
//    width     = attr.width;
//    height    = attr.height;

//    if (!m_useRender || !m_renderSupport) {
//        Pixmap xPixmap = XCompositeNameWindowPixmap(m_display, windowId);
//        //XSync(m_display, 0);
//        pixmap = new QPixmap(QPixmap::fromX11Pixmap(xPixmap, QPixmap::ImplicitlyShared));
//        return pixmap;
//    }
//    else {
//        // TODO: figure out which of the informations restrieved from XGetWindowAttributes
//        // and XRenderFindVisualFormat can be cached, and perhaps invalidated only by
//        // listening from specific X11 events.
//        XGetWindowAttributes(m_display, windowId, &attr);
//        x         = attr.x;
//        y         = attr.y;
//        width     = attr.width;
//        height    = attr.height;

//        format = XRenderFindVisualFormat(m_display, attr.visual);

//        pa.subwindow_mode = IncludeInferiors;
//        picture = XRenderCreatePicture(m_display, windowId, format, CPSubwindowMode, &pa);

//        pixmap = new QPixmap(width, height);


//            pixmap->fill(Qt::transparent);

//            // TODO: figure out if we care about using XFixes do shaped windows or not.
//            // XserverRegion region = XFixesCreateRegionFromWindow(dpy, wId, WindowRegionBounding);
//            // XFixesTranslateRegion(dpy, region, -x, -y);
//            // XFixesSetPictureClipRegion(dpy, picture, 0, 0, region);
//            // XFixesDestroyRegion(dpy, region);

//            // Always use PictOpSrc to copy the pixmap as it ignores alpha, which we don't need.
//            XRenderComposite(m_display, PictOpSrc, picture, None,
//                             pixmap->x11PictureHandle(), 0, 0, 0, 0, 0, 0, width, height);

//        return pixmap;
//    }
}

void WindowGrabber::grabWindow(Window windowId) {
    QPixmap *pixmap = getPixmapForWindow(windowId);
    if (pixmap == 0) return;

    QString outputFileName = m_outputPath + "/" + QString::number(windowId) + ".png";
    if (pixmap->save(outputFileName, "PNG")) {
        qDebug() << "Saved capture:" << outputFileName;
    } else {
        qDebug() << "Failed to save capture:" << outputFileName;
    }
    delete pixmap;
}
