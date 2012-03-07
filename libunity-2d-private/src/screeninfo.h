#ifndef SCREENINFO_H
#define SCREENINFO_H

#include <QObject>
#include <QRect>

class QEvent;
class QWidget;

class ScreenInfo : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QRect geometry READ geometry NOTIFY geometryChanged)
    Q_PROPERTY(QRect availableGeometry READ availableGeometry NOTIFY availableGeometryChanged)
    Q_PROPERTY(QRect panelsFreeGeometry READ panelsFreeGeometry NOTIFY panelsFreeGeometryChanged)
    Q_PROPERTY(int screen READ screen WRITE setScreen NOTIFY screenChanged)
    Q_PROPERTY(QWidget* widget READ widget WRITE setWidget NOTIFY widgetChanged)
    Q_PROPERTY(Corner corner READ corner WRITE setCorner NOTIFY cornerChanged)

public:
    enum Corner {
        InvalidCorner,
        TopLeft,
        TopRight,
        BottomLeft,
        BottomRight
    };

    /* Getters */
    explicit ScreenInfo(QObject *parent = 0);
    ScreenInfo(QWidget* widget, QObject *parent = 0);
    ScreenInfo(int screen, QObject *parent = 0);
    ScreenInfo(Corner corner, QObject *parent = 0);
    ~ScreenInfo();
    QRect availableGeometry() const;
    QRect panelsFreeGeometry() const;
    QRect geometry() const;
    int screen() const;
    QWidget* widget() const;
    Corner corner() const;

    /* Setters */
    void setScreen(int screen);
    void setWidget(QWidget* widget);
    void setCorner(Corner corner);


protected:
    bool eventFilter(QObject *object, QEvent *event);

Q_SIGNALS:
    void geometryChanged(QRect geometry);
    void availableGeometryChanged(QRect availableGeometry);
    void panelsFreeGeometryChanged(QRect panelsFreeGeometry);
    void screenChanged(int screen);
    void widgetChanged(QWidget* widget);
    void cornerChanged(Corner corner);

private Q_SLOTS:
    void updateGeometry(int screen);
    void updateAvailableGeometry(int screen);

private:
    void updateScreen();
    int cornerScreen(Corner corner) const;
    int m_screen;
    QWidget* m_widget;
    Corner m_corner;
};

#endif // SCREENINFO_H
