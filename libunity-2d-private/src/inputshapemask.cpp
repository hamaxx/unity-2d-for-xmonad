#include "inputshapemask.h"
#include "config.h"

#include <QPixmap>
#include <QBitmap>
#include <QDebug>

InputShapeMask::InputShapeMask(QObject *parent) :
    QObject(parent),
    m_enabled(true)
{
}

void InputShapeMask::updateRegion()
{
    QRegion region;

    if (!m_source.isEmpty() && m_color.isValid()) {
        QPixmap image;
        QString path = unity2dDirectory() + m_source;
        if (image.load(path)) {
            region = QRegion(image.createMaskFromColor(m_color.rgb(), Qt::MaskOutColor));
        } else {
            qWarning() << "Failed to load input shape mask image from" << path;
        }
    }

    m_region = region;
    Q_EMIT regionChanged();
}

QString InputShapeMask::source() const
{
    return m_source;
}

QColor InputShapeMask::color() const
{
    return m_color;
}

QPoint InputShapeMask::position() const
{
    return m_position;
}

bool InputShapeMask::enabled() const
{
    return m_enabled;
}

QRegion InputShapeMask::region() const
{
    return m_region;
}

void InputShapeMask::setSource(const QString &source)
{
    if (m_source != source) {
        m_source = source;
        updateRegion();
    }
}

void InputShapeMask::setColor(const QColor &color)
{
    if (m_color != color) {
        m_color = color;
        updateRegion();
    }
}

void InputShapeMask::setPosition(const QPoint &position)
{
    if (m_position != position) {
        m_position = position;
        updateRegion();
    }
}

void InputShapeMask::setEnabled(bool enabled)
{
    if (m_enabled != enabled) {
        m_enabled = enabled;
        Q_EMIT enabledChanged();
    }
}

#include "inputshapemask.moc"
