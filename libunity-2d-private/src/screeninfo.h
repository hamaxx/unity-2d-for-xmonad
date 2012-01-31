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

public:
    /* Getters */
    explicit ScreenInfo(QWidget* widget, QObject *parent = 0);
    ~ScreenInfo();
    QRect availableGeometry() const;
    QRect panelsFreeGeometry() const;
    QRect geometry() const;

protected:
    bool eventFilter(QObject *object, QEvent *event);

Q_SIGNALS:
    void geometryChanged(QRect geometry);
    void availableGeometryChanged(QRect availableGeometry);
    void panelsFreeGeometryChanged(QRect panelsFreeGeometry);

private Q_SLOTS:
    void updateGeometry(int screen);
    void updateAvailableGeometry(int screen);

private:
    void updateScreen();
    int m_screen;
    QWidget* m_widget;
};

#endif // SCREENINFO_H
