#ifndef WINDOWINFO_H
#define WINDOWINFO_H

#include <QObject>
#include <QPoint>
#include <QSize>
#include <QVariant>
#include <QtDeclarative/qdeclarative.h>

class BamfWindow;
class BamfApplication;
typedef struct _WnckWindow WnckWindow;

/* FIXME: position, size, z, title and icon values are not updated real time */
class WindowInfo : public QObject
{
    Q_OBJECT

    Q_PROPERTY(unsigned int xid READ xid WRITE setXid NOTIFY xidChanged)
    Q_PROPERTY(QPoint position READ position NOTIFY positionChanged)
    Q_PROPERTY(QSize size READ size NOTIFY sizeChanged)
    Q_PROPERTY(unsigned int z READ z NOTIFY zChanged)
    Q_PROPERTY(QString title READ title NOTIFY titleChanged)
    Q_PROPERTY(QString icon READ icon NOTIFY iconChanged)

public:
    explicit WindowInfo(unsigned int xid = 0, QObject *parent = 0);

    /* getters */
    unsigned int xid() const;
    QPoint position() const;
    QSize size() const;
    unsigned int z() const;
    QString title() const;
    QString icon() const;

    /* setters */
    void setXid(unsigned int xid);

    Q_INVOKABLE void activate();

    /* FIXME: copied from UnityApplications/launcherapplication.h */
    static void showWindow(WnckWindow* window);
    static void moveViewportToWindow(WnckWindow* window);

Q_SIGNALS:
    void xidChanged(unsigned int xid);
    void positionChanged(QPoint position);
    void sizeChanged(QSize size);
    void zChanged(unsigned int z);
    void titleChanged(QString title);
    void iconChanged(QString icon);

private:
    void updateGeometry();

private:
    BamfApplication *m_bamfApplication;
    BamfWindow *m_bamfWindow;
    WnckWindow *m_wnckWindow;
    unsigned int m_xid;
    QPoint m_position;
    QSize m_size;
};

QML_DECLARE_TYPE(WindowInfo)

#endif // WINDOWINFO_H