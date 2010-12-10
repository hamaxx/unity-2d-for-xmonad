#include <QX11Info>
#include <QDebug>

#include "windowimageprovider.h"

#include <X11/Xlib.h>
#include <X11/extensions/Xcomposite.h>

WindowImageProvider::WindowImageProvider() :
    QDeclarativeImageProvider(QDeclarativeImageProvider::Image) {
}

WindowImageProvider::~WindowImageProvider() {
}

QImage WindowImageProvider::requestImage(const QString &id,
                                              QSize *size,
                                              const QSize &requestedSize) {
    Window win = (Window) id.toULong();
    XWindowAttributes attr;
    XGetWindowAttributes(QX11Info::display(), win, &attr);
    if (attr.map_state != IsViewable) {
        return QImage();
    }

    QPixmap shot = QPixmap::fromX11Pixmap(win);
    if (!shot.isNull()) {
        /* Copy the pixmap to a QImage and then back again to Pixmap. This will create
           a real static copy of the pixmap that's not tied to the server anymore.
           It will be handled by the raster engine *much* faster */
        if (requestedSize.isValid()) {
            shot = shot.scaled(requestedSize);
        }
        size->setWidth(shot.width());
        size->setHeight(shot.height());
        return shot.toImage();
    } else {
        return QImage();
    }

}

/*! Tries to ask the X Composite extension (if supported) to redirect all
    windows on all screens to backing storage. This does not have
    any effect if another application already requested the same
    thing (typically the window manager does this).
*/
void WindowImageProvider::activateComposite() {
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

