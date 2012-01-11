#include "inputshaperectangle.h"

InputShapeRectangle::InputShapeRectangle(QObject *parent) :
    QObject(parent),
    m_enabled(true)
{
}

QRect InputShapeRectangle::rectangle() const
{
    return m_rectangle;
}

void InputShapeRectangle::setRectangle(QRect rectangle)
{
    if (rectangle != m_rectangle) {
        m_rectangle = rectangle;
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

#include "inputshaperectangle.moc"
