#ifndef INPUTSHAPEMANAGER_H
#define INPUTSHAPEMANAGER_H

#include <QObject>
#include <QDeclarativeListProperty>

#include "inputshaperectangle.h"

class InputShapeManager : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QDeclarativeListProperty<InputShapeRectangle> shapes READ shapes)
    Q_CLASSINFO("DefaultProperty", "shapes")

public:
    explicit InputShapeManager(QObject *parent = 0);
    QDeclarativeListProperty<InputShapeRectangle> shapes();

public Q_SLOTS:
    void updateManagedShape();

protected:
    static void appendShape(QDeclarativeListProperty<InputShapeRectangle> *property, InputShapeRectangle *value);

private:
    QList<InputShapeRectangle*> m_shapes;
};

#endif // INPUTSHAPEMANAGER_H
