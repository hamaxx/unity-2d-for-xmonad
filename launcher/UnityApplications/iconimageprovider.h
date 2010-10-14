#ifndef ICONIMAGEPROVIDER_H
#define ICONIMAGEPROVIDER_H

#include <QDeclarativeImageProvider>
#include <QHash>

class IconImageProvider : public QDeclarativeImageProvider
{
public:
    IconImageProvider();
    ~IconImageProvider();
    virtual QImage requestImage(const QString &id, QSize *size, const QSize &requestedSize);

private:
    /* Cache of Gtk themes */
    /* FIXME: values are set to be of type void* in order to avoid importing
              gtk/gtk.h necessary for GtkIconTheme that would otherwise lead
              to conflicts with Qt keywords (signals, slots, etc.).
    */
    QHash<QString, void*> m_themes;
};

#endif // ICONIMAGEPROVIDER_H
