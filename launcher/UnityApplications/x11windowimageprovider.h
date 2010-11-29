#ifndef X11WINDOWIMAGEPROVIDER_H
#define X11WINDOWIMAGEPROVIDER_H

#include <QDeclarativeImageProvider>
#include <QPixmap>
#include <QSize>

class WindowGrabber;

class X11WindowImageProvider : public QDeclarativeImageProvider
{
public:
    X11WindowImageProvider();
    ~X11WindowImageProvider();
    virtual QPixmap requestPixmap(const QString &id, QSize *size, const QSize &requestedSize);

private:
    WindowGrabber *m_capture;
};

#endif // X11WINDOWIMAGEPROVIDER_H
