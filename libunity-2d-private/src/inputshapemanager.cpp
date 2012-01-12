#include <QDebug>
#include <QRect>
#include <QX11Info>
#include <QRegion>

#include "inputshapemanager.h"
#include "unity2ddeclarativeview.h"
#include "screeninfo.h"

// X11
#include <X11/Xlib.h>
#include <X11/Xregion.h>
#include <X11/extensions/shape.h>

InputShapeManager::InputShapeManager(QObject *parent) :
    QObject(parent),
    m_target(0)
{
}

void InputShapeManager::updateManagedShape()
{
    if (m_target == NULL || !m_target->isVisible()) {
        return;
    }

    QRegion region;
    Q_FOREACH(InputShapeRectangle* shape, m_shapes) {
        if (shape->enabled()) {
            region += shape->region();
        }
    }

    XShapeCombineRegion(QX11Info::display(), m_target->effectiveWinId(), ShapeInput,
                        0, 0, region.handle(), ShapeSet);
}

Unity2DDeclarativeView* InputShapeManager::target() const
{
    return m_target;
}

void InputShapeManager::setTarget(Unity2DDeclarativeView *target)
{
    if (m_target != target) {
        if (m_target != NULL) m_target->disconnect(this);

        //TODO: de-shape the current target view, if any, before shaping the new one. not used now
        //      as we never change the view anyway.

        m_target = target;
        if (m_target != NULL) {
            // due to the way xshape works we need to re-apply the shaping every time the target window
            // is mapped again.
            connect(m_target, SIGNAL(shown()), SLOT(updateManagedShape()));
        }
        Q_EMIT targetChanged();
        updateManagedShape();
    }
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
        instance->connect(shape, SIGNAL(regionChanged()), SLOT(updateManagedShape()));
        instance->connect(shape, SIGNAL(enabledChanged()), SLOT(updateManagedShape()));
        instance->updateManagedShape();
    }
}

#include "inputshapemanager.moc"
