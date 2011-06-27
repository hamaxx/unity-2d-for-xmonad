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
#include <fakecairo.h>
#include <panelstyle.h>

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
, m_padding(PADDING)
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
        if (m_entry->active()) {
            paintActiveBackground(&painter);
        }
        painter.drawPixmap(0, 0, m_pix);
    }
}


void IndicatorEntryWidget::paintActiveBackground(QPainter* painter)
{
    // This code should be kept in sync with the draw_menu_bg() function from
    // plugins/unityshell/src/PanelIndicatorObjectEntryView.cpp
    int radius = 4;
    double x = 0;
    double y = 0;
    double xos = 0.5;
    double yos = 0.5;
    /* FIXME */
    double mpi = 3.14159265358979323846;

    nux::Color bgtop = PanelStyle::instance()->backgroundTopColor();
    nux::Color bgbot = PanelStyle::instance()->backgroundBottomColor();
    nux::Color line = PanelStyle::instance()->lineColor();

    painter->setRenderHint(QPainter::Antialiasing);

    fake_cairo_t cr(painter);

    cairo_move_to (cr, x+xos+radius, y+yos);
    cairo_arc (cr, x+xos+width()-xos*2-radius, y+yos+radius, radius, mpi*1.5, mpi*2);
    cairo_line_to (cr, x+xos+width()-xos*2, y+yos+height()-yos*2+2);
    cairo_line_to (cr, x+xos, y+yos+height()-yos*2+2);
    cairo_arc (cr, x+xos+radius, y+yos+radius, radius, mpi, mpi*1.5);

    cairo_pattern_t * pat = cairo_pattern_create_linear (x+xos, y, x+xos, y+height()-yos*2+2);
    cairo_pattern_add_color_stop_rgba (pat, 0.0,
                                     bgtop.red,
                                     bgtop.green,
                                     bgtop.blue,
                                     1.0f - bgbot.red);
    cairo_pattern_add_color_stop_rgba (pat, 1.0,
                                     bgbot.red,
                                     bgbot.green,
                                     bgbot.blue,
                                     1.0f - bgtop.red);
    cairo_set_source (cr, pat);
    cairo_fill_preserve (cr);
    cairo_pattern_destroy (pat);

    /*
    pat = cairo_pattern_create_linear (x+xos, y, x+xos, y+height()-yos*2+2);
    cairo_pattern_add_color_stop_rgba (pat, 0.0,
                                     line.red,
                                     line.green,
                                     line.blue,
                                     1.0f);
    cairo_pattern_add_color_stop_rgba (pat, 1.0,
                                     line.red,
                                     line.green,
                                     line.blue,
                                     1.0f);
    cairo_set_source (cr, pat);
    */
    cairo_set_source_rgb (cr, line.red, line.green, line.blue);
    cairo_stroke (cr);
    //cairo_pattern_destroy (pat);

    xos++;
    yos++;

    /* enlarging the area to not draw the lightborder at bottom, ugly trick :P */
    cairo_move_to (cr, x+radius+xos, y+yos);
    cairo_arc (cr, x+xos+width()-xos*2-radius, y+yos+radius, radius, mpi*1.5, mpi*2);
    cairo_line_to (cr, x+xos+width()-xos*2, y+yos+height()-yos*2+3);
    cairo_line_to (cr, x+xos, y+yos+height()-yos*2+3);
    cairo_arc (cr, x+xos+radius, y+yos+radius, radius, mpi, mpi*1.5);

    /*
    pat = cairo_pattern_create_linear (x+xos, y, x+xos, y+height()-yos*2+3);
    cairo_pattern_add_color_stop_rgba (pat, 0.0,
                                     bgbot.red,
                                     bgbot.green,
                                     bgbot.blue,
                                     1.0f);
    cairo_pattern_add_color_stop_rgba (pat, 1.0,
                                     bgbot.red,
                                     bgbot.green,
                                     bgbot.blue,
                                     1.0f);
    cairo_set_source (cr, pat);
    */
    cairo_set_source_rgb (cr, bgbot.red, bgbot.green, bgbot.blue);
    cairo_stroke (cr);
    //cairo_pattern_destroy (pat);
}

void IndicatorEntryWidget::updatePix()
{
    int width = m_padding;
    int iconX = m_padding;
    int labelX = 0;
    bool hasIcon = false;
    bool hasLabel = false;

    PanelStyle* style = PanelStyle::instance();

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
        width += QFontMetrics(style->font()).width(label);
    }

    width += m_padding;

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
            bool disabled = !m_entry->image_sensitive();
            if (disabled) {
                painter.setOpacity(0.5);
            }
            painter.drawPixmap(iconX, 0, iconPix);
            if (disabled) {
                painter.setOpacity(1);
            }
        }
        if (hasLabel) {
            painter.setFont(style->font());

            // Shadow
            QColor color = style->textShadowColor();
            color.setAlphaF(1. - color.redF());
            painter.setPen(color);
            painter.drawText(labelX, 1, width - labelX, m_pix.height(), Qt::AlignLeft | Qt::AlignVCenter, label);

            // Text
            color = style->textColor();
            color.setAlphaF(m_entry->label_sensitive() ? 1. : .5);
            painter.setPen(color);
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

void IndicatorEntryWidget::setPadding(int padding)
{
    if (m_padding != padding) {
        m_padding = padding;
        updatePix();
    }
}

#include "indicatorentrywidget.moc"
