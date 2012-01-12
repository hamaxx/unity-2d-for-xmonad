#include "inputshaperectangle.h"
#include "inputshapemask.h"

#include <QBitmap>
#include <QPainter>
#include <QPainterPath>
#include <QDebug>
#include <QRect>

InputShapeRectangle::InputShapeRectangle(QObject *parent) :
    QObject(parent),
    m_enabled(true)
{
}

void InputShapeRectangle::updateRegion()
{
    QRegion newRegion(m_rectangle);

    Q_FOREACH (InputShapeMask* mask, m_masks) {
        if (mask->enabled()) {
            newRegion -= mask->region().translated(mask->position());
        }
    }

    m_region = newRegion;
    Q_EMIT regionChanged();
}

QRect InputShapeRectangle::rectangle() const
{
    return m_rectangle;
}

void InputShapeRectangle::setRectangle(QRect rectangle)
{
    if (rectangle != m_rectangle) {
        m_rectangle = rectangle;
        updateRegion();
        Q_EMIT rectangleChanged();
    }
}

bool InputShapeRectangle::enabled() const
{
    return m_enabled;
}

void InputShapeRectangle::setEnabled(bool enabled)
{
    if (enabled != m_enabled) {
        m_enabled = enabled;
        Q_EMIT enabledChanged();

    }
}

QRegion InputShapeRectangle::region() const
{
    return m_region;
}

QDeclarativeListProperty<InputShapeMask> InputShapeRectangle::masks()
{
    return QDeclarativeListProperty<InputShapeMask>(this, this, &InputShapeRectangle::appendMask);
}

void InputShapeRectangle::appendMask(QDeclarativeListProperty<InputShapeMask> *list, InputShapeMask *mask)
{
    InputShapeRectangle* instance = qobject_cast<InputShapeRectangle*>(list->object);
    if (instance != NULL) {
        instance->m_masks.append(mask);
        instance->connect(mask, SIGNAL(enabledChanged()), SLOT(foo()));
        instance->connect(mask, SIGNAL(regionChanged()), SLOT(updateRegion()));
        instance->updateRegion();
    }
}

void InputShapeRectangle::foo()
{
    qDebug() << "ENABLE changed" << qobject_cast<InputShapeMask*>(sender())->enabled();
    updateRegion();
}

#include "inputshaperectangle.moc"
