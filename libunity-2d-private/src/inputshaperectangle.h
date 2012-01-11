#ifndef INPUTSHAPERECTANGLE_H
#define INPUTSHAPERECTANGLE_H

#include <QObject>
#include <QRect>

class InputShapeRectangle : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QRect rectangle READ rectangle WRITE setRectangle NOTIFY rectangleChanged)
    Q_PROPERTY(bool enabled READ enabled WRITE setEnabled NOTIFY enabledChanged)

public:
    explicit InputShapeRectangle(QObject *parent = 0);

    QRect rectangle() const;
    void setRectangle(QRect rectangle);
    bool enabled() const;
    void setEnabled(bool enabled);

Q_SIGNALS:
    void rectangleChanged();
    void enabledChanged();

private:
    QRect m_rectangle;
    bool m_enabled;
};

#endif // INPUTSHAPERECTANGLE_H
