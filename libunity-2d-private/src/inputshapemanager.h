#ifndef INPUTSHAPEMANAGER_H
#define INPUTSHAPEMANAGER_H

#include <QObject>
#include <QDeclarativeListProperty>

#include "inputshaperectangle.h"

class Unity2DDeclarativeView;

class InputShapeManager : public QObject
{
    Q_OBJECT
    Q_PROPERTY(Unity2DDeclarativeView* target READ target WRITE setTarget NOTIFY targetChanged)
    Q_PROPERTY(QDeclarativeListProperty<InputShapeRectangle> shapes READ shapes)
    Q_CLASSINFO("DefaultProperty", "shapes")

public:
    explicit InputShapeManager(QObject *parent = 0);
    Unity2DDeclarativeView* target() const;
    void setTarget(Unity2DDeclarativeView* target);
    QDeclarativeListProperty<InputShapeRectangle> shapes();

Q_SIGNALS:
    void targetChanged();

public Q_SLOTS:
    void updateManagedShape();

protected:
    static void appendShape(QDeclarativeListProperty<InputShapeRectangle> *property, InputShapeRectangle *value);

private:
    Unity2DDeclarativeView* m_target;
    QList<InputShapeRectangle*> m_shapes;
};

#endif // INPUTSHAPEMANAGER_H
