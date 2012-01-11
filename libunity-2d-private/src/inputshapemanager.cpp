#include "inputshapemanager.h"

#include <QDebug>
#include <QRegion>
#include <QRect>

InputShapeManager::InputShapeManager(QObject *parent) :
    QObject(parent)
{
}

void InputShapeManager::updateManagedShape()
{
    QRegion region;
    Q_FOREACH(InputShapeRectangle* shape, m_shapes) {
        if (shape->enabled()) {
            region += shape->rectangle();
        }
    }

    qDebug() << "NEW REGION" << region.boundingRect();
}

QDeclarativeListProperty<InputShapeRectangle> InputShapeManager::shapes()
{
    return QDeclarativeListProperty<InputShapeRectangle>(this, this, &InputShapeManager::appendShape);
}

void InputShapeManager::appendShape(QDeclarativeListProperty<InputShapeRectangle> *list, InputShapeRectangle *shape)
{
    InputShapeManager* instance = qobject_cast<InputShapeManager*>(list->object);
    if (instance != NULL) {
        instance->m_shapes.append(shape);
        instance->connect(shape, SIGNAL(rectangleChanged()), SLOT(updateManagedShape()));
        instance->connect(shape, SIGNAL(enabledChanged()), SLOT(updateManagedShape()));
        instance->updateManagedShape();
    }
}

#include "inputshapemanager.moc"
