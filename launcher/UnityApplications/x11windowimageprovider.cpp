#include <QDebug>

#include "x11windowimageprovider.h"
#include "windowgrabber.h"


X11WindowImageProvider::X11WindowImageProvider() :
    QDeclarativeImageProvider(QDeclarativeImageProvider::Pixmap),
    m_capture(new WindowGrabber()){
}

X11WindowImageProvider::~X11WindowImageProvider() {
    delete m_capture;
}

QPixmap X11WindowImageProvider::requestPixmap(const QString &id,
                                              QSize *size,
                                              const QSize &requestedSize) {
    QPixmap *shot = m_capture->getPixmapForWindow((Window)id.toULong());
    size->setWidth(shot->width());
    size->setHeight(shot->height());
    qDebug() << "+++++++++++++" << id << requestedSize << shot->width() << " x " << shot->height();
    return *shot;
}
