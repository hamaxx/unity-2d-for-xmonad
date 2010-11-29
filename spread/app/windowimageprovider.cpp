#include <QDebug>
#include <QApplication>

#include "windowimageprovider.h"
#include "windowgrabber.h"


WindowImageProvider::WindowImageProvider(bool useX11) :
    QDeclarativeImageProvider((useX11) ?
                               QDeclarativeImageProvider::Pixmap : QDeclarativeImageProvider::Image),
    m_capture(new WindowGrabber()) {
    qDebug() << "Using image type: " << ((imageType() == QDeclarativeImageProvider::Pixmap) ? "Pixmap" : "Image");
}

WindowImageProvider::~WindowImageProvider() {
    delete m_capture;
}

QPixmap WindowImageProvider::requestPixmap(const QString &id,
                                              QSize *size,
                                              const QSize &requestedSize) {
    QPixmap *shot = m_capture->getPixmapForWindow((Window)id.toULong());
    if (shot) {
        size->setWidth(shot->width());
        size->setHeight(shot->height());
        return *shot;
    } else {
        return QPixmap();
    }

}

QImage WindowImageProvider::requestImage(const QString &id,
                                            QSize *size,
                                            const QSize &requestedSize) {
    QImage *shot = m_capture->getImageForWindow((Window)id.toULong());
    if (shot) {
        size->setWidth(shot->width());
        size->setHeight(shot->height());
        return *shot;
    } else {
        return QImage();
    }
}

