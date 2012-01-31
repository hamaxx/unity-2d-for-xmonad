#ifndef SCREENINFO_H
#define SCREENINFO_H

#include <QObject>
#include <QRect>

class ScreenInfo : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QRect geometry READ geometry NOTIFY geometryChanged)
    Q_PROPERTY(QRect availableGeometry READ availableGeometry NOTIFY availableGeometryChanged)
    Q_PROPERTY(QRect panelsFreeGeometry READ panelsFreeGeometry NOTIFY panelsFreeGeometryChanged)

public:
    /* Getters */
    explicit ScreenInfo(QObject *parent = 0);
    QRect availableGeometry() const;
    QRect panelsFreeGeometry() const;
    QRect geometry() const;

Q_SIGNALS:
    void geometryChanged(QRect geometry);
    void availableGeometryChanged(QRect availableGeometry);
    void panelsFreeGeometryChanged(QRect panelsFreeGeometry);

private Q_SLOTS:
    void updateGeometry(int screen);
    void updateAvailableGeometry(int screen);
};

#endif // SCREENINFO_H
