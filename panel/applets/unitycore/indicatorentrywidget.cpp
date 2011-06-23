/*
 * This file is part of unity-2d
 *
 * Copyright 2011 Canonical Ltd.
 *
 * Authors:
 * - Aurélien Gâteau <aurelien.gateau@canonical.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 3.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
// Self
#include "indicatorentrywidget.h"

// Local
#include <debug_p.h>

// Qt
#include <QIcon>
#include <QPainter>
#include <QWheelEvent>

// libc
#include <time.h>

static const int SPACING = 3;
static const int PADDING = 3;

using namespace unity::indicator;

IndicatorEntryWidget::IndicatorEntryWidget(const Entry::Ptr& entry)
: m_entry(entry)
{
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Minimum);
    m_entry->updated.connect(sigc::mem_fun(this, &IndicatorEntryWidget::updatePix));
}

QSize IndicatorEntryWidget::minimumSizeHint() const
{
    return sizeHint();
}

QSize IndicatorEntryWidget::sizeHint() const
{
    return m_pix.size();
}

void IndicatorEntryWidget::paintEvent(QPaintEvent*)
{
    if (!m_pix.isNull()) {
        QPainter painter(this);
        painter.drawPixmap(0, 0, m_pix);
    }
}

void IndicatorEntryWidget::updatePix()
{
    int width = PADDING;
    int iconX = PADDING;
    int labelX = 0;
    bool hasIcon = false;
    bool hasLabel = false;

    // Compute width, labelX and has{Icon,Label}
    QPixmap iconPix = decodeIcon();
    if (!iconPix.isNull()) {
        hasIcon = true;
        width += iconPix.width();
    }

    QString label = QString::fromStdString(m_entry->label());
    label.replace("_", "");
    if (!label.isEmpty()) {
        hasLabel = true;
        if (hasIcon) {
            width += SPACING;
        }
        labelX = width;
        width += fontMetrics().width(label);
    }

    width += PADDING;

    // Paint
    QPixmap oldPix = m_pix;
    if (!hasIcon && !hasLabel) {
        m_pix = QPixmap();
    } else {
        m_pix = QPixmap(width, 24);
        m_pix.fill(Qt::transparent);
        QPainter painter(&m_pix);
        painter.initFrom(this);
        if (hasIcon) {
            painter.drawPixmap(iconX, 0, iconPix);
        }
        if (hasLabel) {
            painter.drawText(labelX, 0, width - labelX, m_pix.height(), Qt::AlignLeft | Qt::AlignVCenter, label);
        }
    }

    // Notify others we changed, but only trigger a layout update if necessary
    if (m_pix.size() == oldPix.size()) {
        update();
    } else {
        updateGeometry();
    }
}

QPixmap IndicatorEntryWidget::decodeIcon()
{
    QPixmap pix;

    int type = m_entry->image_type();

    if (type == 0) {
        // No icon
    } else if (type == 3) {
        // GTK_IMAGE_PIXBUF
        QByteArray data = QByteArray::fromBase64(m_entry->image_data().c_str());
        QImage image;
        bool ok = image.loadFromData(data);
        if (ok) {
            pix = QPixmap::fromImage(image);
        } else {
            UQ_WARNING << "Failed to decode image";
        }
    } else if (type == 7) {
        // GTK_IMAGE_ICON_NAME
        QString name = QString::fromStdString(m_entry->image_data());
        QIcon icon = QIcon::fromTheme(name);
        pix = icon.pixmap(24, 24);
    } else if (type == 8) {
        // GTK_IMAGE_GICON
        UQ_WARNING << "FIXME" << type;
    } else {
        UQ_WARNING << "Unknown image type" << m_entry->image_type();
    }
    return pix;
}

void IndicatorEntryWidget::mousePressEvent(QMouseEvent*)
{
    if (m_entry->active()) {
        return;
    }

    if ((m_entry->label_visible() && m_entry->label_sensitive()) ||
        (m_entry->image_visible() && m_entry->image_sensitive()))
    {
        showMenu();
    } else {
        update();
    }
}

void IndicatorEntryWidget::mouseReleaseEvent(QMouseEvent*)
{
    update();
}

void IndicatorEntryWidget::wheelEvent(QWheelEvent* event)
{
    m_entry->Scroll(event->delta());
}

void IndicatorEntryWidget::showMenu()
{
    QPoint pos = mapToGlobal(rect().bottomLeft());
    m_entry->ShowMenu(pos.x(), pos.y(),
        time(NULL),
        1 //nux::GetEventButton(button_flags)
        );
}

#include "indicatorentrywidget.moc"
