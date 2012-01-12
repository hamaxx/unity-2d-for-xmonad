#ifndef INPUTSHAPERECTANGLE_H
#define INPUTSHAPERECTANGLE_H

#include <QObject>
#include <QRect>
#include <QRegion>
#include <QDeclarativeListProperty>
#include <QList>

class InputShapeMask;

class InputShapeRectangle : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QRect rectangle READ rectangle WRITE setRectangle NOTIFY rectangleChanged)
    Q_PROPERTY(bool enabled READ enabled WRITE setEnabled NOTIFY enabledChanged)
    Q_PROPERTY(QRegion region READ region NOTIFY regionChanged)
    Q_PROPERTY(QDeclarativeListProperty<InputShapeMask> masks READ masks)
    Q_CLASSINFO("DefaultProperty", "masks")

public:
    explicit InputShapeRectangle(QObject *parent = 0);

    QRect rectangle() const;
    void setRectangle(QRect rectangle);
    bool enabled() const;
    void setEnabled(bool enabled);
    QRegion region() const;
    QDeclarativeListProperty<InputShapeMask> masks();

protected:
    static void appendMask(QDeclarativeListProperty<InputShapeMask> *list, InputShapeMask *mask);

protected Q_SLOTS:
    void updateRegion();
    void foo();

Q_SIGNALS:
    void rectangleChanged();
    void enabledChanged();
    void regionChanged();

private:
    QRect m_rectangle;
    bool m_enabled;
    QRegion m_region;
    QList<InputShapeMask*> m_masks;
};

#endif // INPUTSHAPERECTANGLE_H
