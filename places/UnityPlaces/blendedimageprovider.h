#ifndef BLENDEDIMAGEPROVIDER_H
#define BLENDEDIMAGEPROVIDER_H

#include <QDeclarativeImageProvider>

class BlendedImageProvider : public QDeclarativeImageProvider
{
public:
    BlendedImageProvider();
    ~BlendedImageProvider();
    virtual QImage requestImage(const QString &id, QSize *size, const QSize &requestedSize);
};

#endif // BLENDEDIMAGEPROVIDER_H
