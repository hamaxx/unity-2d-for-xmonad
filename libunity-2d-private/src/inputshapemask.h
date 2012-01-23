#ifndef INPUTSHAPEMASK_H
#define INPUTSHAPEMASK_H

#include <QObject>
#include <QColor>
#include <QPoint>
#include <QBitmap>

class InputShapeMask : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString source READ source WRITE setSource)
    Q_PROPERTY(QColor color READ color WRITE setColor)
    Q_PROPERTY(QPoint position READ position WRITE setPosition NOTIFY positionChanged)
    Q_PROPERTY(QBitmap shape READ shape NOTIFY shapeChanged)
    Q_PROPERTY(bool enabled READ enabled WRITE setEnabled NOTIFY enabledChanged)

public:
    explicit InputShapeMask(QObject *parent = 0);

    QString source() const;
    QColor color() const;
    QPoint position() const;
    bool enabled() const;
    QBitmap shape() const;

    void setSource(const QString& source);
    void setColor(const QColor& color);
    void setPosition(const QPoint& position);
    void setEnabled(bool enabled);

Q_SIGNALS:
    void enabledChanged();
    void shapeChanged();
    void positionChanged();

protected:
    void updateShape();

private:
    QString m_source;
    QColor m_color;
    QPoint m_position;
    bool m_enabled;
    QBitmap m_shape;
};

#endif // INPUTSHAPEMASK_H
