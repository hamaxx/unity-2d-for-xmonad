#ifndef X11WINDOWIMAGEPROVIDER_H
#define X11WINDOWIMAGEPROVIDER_H

#include <QDeclarativeImageProvider>
#include <QPixmap>
#include <QImage>
#include <QSize>
#include <QPaintEngine>

class WindowGrabber;

class WindowImageProvider : public QDeclarativeImageProvider
{
public:
    WindowImageProvider(bool useX11);
    ~WindowImageProvider();
    virtual QPixmap requestPixmap(const QString &id, QSize *size, const QSize &requestedSize);
    virtual QImage requestImage(const QString &id, QSize *size, const QSize &requestedSize);

private:
    WindowGrabber *m_capture;
};

#endif // X11WINDOWIMAGEPROVIDER_H
