#ifndef INPUTSHAPEMASK_H
#define INPUTSHAPEMASK_H

#include <QObject>
#include <QColor>
#include <QPoint>
#include <QRegion>

class InputShapeMask : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString source READ source WRITE setSource)
    Q_PROPERTY(QColor color READ color WRITE setColor)
    Q_PROPERTY(QPoint position READ position WRITE setPosition)
    Q_PROPERTY(bool enabled READ enabled WRITE setEnabled NOTIFY enabledChanged)
    Q_PROPERTY(QRegion region READ region NOTIFY regionChanged)

public:
    explicit InputShapeMask(QObject *parent = 0);

    QString source() const;
    QColor color() const;
    QPoint position() const;
    bool enabled() const;
    QRegion region() const;

    void setSource(const QString& source);
    void setColor(const QColor& color);
    void setPosition(const QPoint& position);
    void setEnabled(bool enabled);

Q_SIGNALS:
    void enabledChanged();
    void regionChanged();

protected:
    void updateRegion();

private:
    QString m_source;
    QColor m_color;
    QPoint m_position;
    bool m_enabled;
    QRegion m_region;
};

#endif // INPUTSHAPEMASK_H
