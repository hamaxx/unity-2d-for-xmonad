#include <QX11Info>
#include <QDebug>

#include "windowimageprovider.h"

#include <X11/Xlib.h>
#include <X11/extensions/Xcomposite.h>

WindowImageProvider::WindowImageProvider() :
    QDeclarativeImageProvider(QDeclarativeImageProvider::Pixmap) {
}

WindowImageProvider::~WindowImageProvider() {
}

QPixmap WindowImageProvider::requestPixmap(const QString &id,
                                              QSize *size,
                                              const QSize &requestedSize) {
    QPixmap shot = QPixmap::fromX11Pixmap((Qt::HANDLE)id.toULong());
    if (!shot.isNull()) {
        if (requestedSize.isValid()) {
            shot = shot.scaled(requestedSize);
        }
        size->setWidth(shot.width());
        size->setHeight(shot.height());
        return shot;
    } else {
        return QPixmap();
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

