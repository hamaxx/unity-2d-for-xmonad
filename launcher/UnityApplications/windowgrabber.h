#ifndef WINDOWGRABBER_H
#define WINDOWGRABBER_H

#include <QObject>
#include <QPixmap>

#include <X11/Xlib.h>

class WindowGrabber : public QObject {
    Q_OBJECT

    Q_PROPERTY(QString outputPath READ outputPath WRITE setOutputPath)
    Q_PROPERTY(bool useRender READ useRender WRITE setUseRender)

public:
    explicit WindowGrabber(QObject *parent = 0);
    ~WindowGrabber();
    QPixmap* getPixmapForWindow(Window windowId);

    QString outputPath() const;
    void setOutputPath(const QString path);

    bool useRender() const { return m_useRender; }
    void setUseRender(bool useRender) { m_useRender = useRender; }

public slots:
    void grabWindow(Window windowId);

private:
    Display *m_display;
    QString m_outputPath;
    bool m_compositeSupport;
    bool m_renderSupport;
    bool m_useRender;
};

#endif // WINDOWGRABBER_H
