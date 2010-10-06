#ifndef ICONIMAGEPROVIDER_H
#define ICONIMAGEPROVIDER_H

#include <QDeclarativeImageProvider>

class IconImageProvider : public QDeclarativeImageProvider
{
public:
    IconImageProvider();
    virtual QImage requestImage(const QString &id, QSize *size, const QSize &requestedSize);

signals:

public slots:

};

#endif // ICONIMAGEPROVIDER_H
