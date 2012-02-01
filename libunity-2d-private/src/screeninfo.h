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
    Q_PROPERTY(int screen READ screen NOTIFY screenChanged)
    Q_PROPERTY(QWidget* widget READ widget NOTIFY widgetChanged)

public:
    /* Getters */
    explicit ScreenInfo(QObject *parent = 0);
    ScreenInfo(QWidget* widget, QObject *parent = 0);
    ScreenInfo(int screen, QObject *parent = 0);
    ~ScreenInfo();
    QRect availableGeometry() const;
    QRect panelsFreeGeometry() const;
    QRect geometry() const;
    int screen() const;
    QWidget* widget() const;

    /* Setters */
    void setScreen(int screen);
    void setWidget(QWidget* widget);

protected:
    bool eventFilter(QObject *object, QEvent *event);

Q_SIGNALS:
    void geometryChanged(QRect geometry);
    void availableGeometryChanged(QRect availableGeometry);
    void panelsFreeGeometryChanged(QRect panelsFreeGeometry);
    void screenChanged(int screen);
    void widgetChanged(QWidget* widget);

private Q_SLOTS:
    void updateGeometry(int screen);
    void updateAvailableGeometry(int screen);

private:
    void updateScreen();
    int m_screen;
    QWidget* m_widget;
};

#endif // SCREENINFO_H
