#ifndef INPUTSHAPERECTANGLE_H
#define INPUTSHAPERECTANGLE_H

#include <QObject>
#include <QRect>
#include <QBitmap>
#include <QDeclarativeListProperty>
#include <QList>

class InputShapeMask;

class InputShapeRectangle : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QRect rectangle READ rectangle WRITE setRectangle NOTIFY rectangleChanged)
    Q_PROPERTY(bool enabled READ enabled WRITE setEnabled NOTIFY enabledChanged)
    Q_PROPERTY(QBitmap shape READ shape NOTIFY shapeChanged)
    Q_PROPERTY(QDeclarativeListProperty<InputShapeMask> masks READ masks)
    Q_CLASSINFO("DefaultProperty", "masks")

public:
    explicit InputShapeRectangle(QObject *parent = 0);

    QRect rectangle() const;
    void setRectangle(QRect rectangle);
    bool enabled() const;
    void setEnabled(bool enabled);
    QBitmap shape() const;
    QDeclarativeListProperty<InputShapeMask> masks();

protected:
    static void appendMask(QDeclarativeListProperty<InputShapeMask> *list, InputShapeMask *mask);

protected Q_SLOTS:
    void updateShape();

Q_SIGNALS:
    void rectangleChanged();
    void enabledChanged();
    void shapeChanged();

private:
    QRect m_rectangle;
    bool m_enabled;
    QBitmap m_shape;
    QList<InputShapeMask*> m_masks;
};

#endif // INPUTSHAPERECTANGLE_H
