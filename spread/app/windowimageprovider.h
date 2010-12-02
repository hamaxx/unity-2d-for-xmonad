#ifndef X11WINDOWIMAGEPROVIDER_H
#define X11WINDOWIMAGEPROVIDER_H

#include <QDeclarativeImageProvider>
#include <QPixmap>
#include <QImage>
#include <QSize>

class WindowGrabber;

class WindowImageProvider : public QDeclarativeImageProvider
{
public:
    WindowImageProvider();
    ~WindowImageProvider();
    virtual QPixmap requestPixmap(const QString &id, QSize *size, const QSize &requestedSize);
    static void activateComposite();
};

#endif // X11WINDOWIMAGEPROVIDER_H
