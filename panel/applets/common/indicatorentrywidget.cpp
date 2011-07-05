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

// GTK
#include <gtk/gtk.h>

// libc
#include <time.h>

static const int SPACING = 3;
static const int PADDING = 3;

using namespace unity::indicator;

// Copied from libdbusmenu-qt
static QString swapMnemonicChar(const QString &in, const char src, const char dst)
{
    QString out;
    bool mnemonicFound = false;

    for (int pos = 0; pos < in.length(); ) {
        QChar ch = in[pos];
        if (ch == src) {
            if (pos == in.length() - 1) {
                // 'src' at the end of string, skip it
                ++pos;
            } else {
                if (in[pos + 1] == src) {
                    // A real 'src'
                    out += src;
                    pos += 2;
                } else if (!mnemonicFound) {
                    // We found the mnemonic
                    mnemonicFound = true;
                    out += dst;
                    ++pos;
                } else {
                    // We already have a mnemonic, just skip the char
                    ++pos;
                }
            }
        } else if (ch == dst) {
            // Escape 'dst'
            out += dst;
            out += dst;
            ++pos;
        } else {
            out += ch;
            ++pos;
        }
    }

    return out;
}

IndicatorEntryWidget::IndicatorEntryWidget(const Entry::Ptr& entry)
: m_entry(entry)
, m_padding(PADDING)
, m_hasIcon(false)
, m_hasLabel(false)
{
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Minimum);
    m_entry->updated.connect(sigc::mem_fun(this, &IndicatorEntryWidget::updatePix));
    updatePix();
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

    fcairo_t cr(painter);

    fcairo_move_to (cr, x+xos+radius, y+yos);
    fcairo_arc (cr, x+xos+width()-xos*2-radius, y+yos+radius, radius, mpi*1.5, mpi*2);
    fcairo_line_to (cr, x+xos+width()-xos*2, y+yos+height()-yos*2+2);
    fcairo_line_to (cr, x+xos, y+yos+height()-yos*2+2);
    fcairo_arc (cr, x+xos+radius, y+yos+radius, radius, mpi, mpi*1.5);

    fcairo_pattern_t * pat = fcairo_pattern_create_linear (x+xos, y, x+xos, y+height()-yos*2+2);
    fcairo_pattern_add_color_stop_rgba (pat, 0.0,
                                     bgtop.red,
                                     bgtop.green,
                                     bgtop.blue,
                                     1.0f - bgbot.red);
    fcairo_pattern_add_color_stop_rgba (pat, 1.0,
                                     bgbot.red,
                                     bgbot.green,
                                     bgbot.blue,
                                     1.0f - bgtop.red);
    fcairo_set_source (cr, pat);
    fcairo_fill_preserve (cr);
    fcairo_pattern_destroy (pat);

    /*
    pat = fcairo_pattern_create_linear (x+xos, y, x+xos, y+height()-yos*2+2);
    fcairo_pattern_add_color_stop_rgba (pat, 0.0,
                                     line.red,
                                     line.green,
                                     line.blue,
                                     1.0f);
    fcairo_pattern_add_color_stop_rgba (pat, 1.0,
                                     line.red,
                                     line.green,
                                     line.blue,
                                     1.0f);
    fcairo_set_source (cr, pat);
    */
    fcairo_set_source_rgb (cr, line.red, line.green, line.blue);
    fcairo_stroke (cr);
    //fcairo_pattern_destroy (pat);

    xos++;
    yos++;

    /* enlarging the area to not draw the lightborder at bottom, ugly trick :P */
    fcairo_move_to (cr, x+radius+xos, y+yos);
    fcairo_arc (cr, x+xos+width()-xos*2-radius, y+yos+radius, radius, mpi*1.5, mpi*2);
    fcairo_line_to (cr, x+xos+width()-xos*2, y+yos+height()-yos*2+3);
    fcairo_line_to (cr, x+xos, y+yos+height()-yos*2+3);
    fcairo_arc (cr, x+xos+radius, y+yos+radius, radius, mpi, mpi*1.5);

    /*
    pat = fcairo_pattern_create_linear (x+xos, y, x+xos, y+height()-yos*2+3);
    fcairo_pattern_add_color_stop_rgba (pat, 0.0,
                                     bgbot.red,
                                     bgbot.green,
                                     bgbot.blue,
                                     1.0f);
    fcairo_pattern_add_color_stop_rgba (pat, 1.0,
                                     bgbot.red,
                                     bgbot.green,
                                     bgbot.blue,
                                     1.0f);
    fcairo_set_source (cr, pat);
    */
    fcairo_set_source_rgb (cr, bgbot.red, bgbot.green, bgbot.blue);
    fcairo_stroke (cr);
    //fcairo_pattern_destroy (pat);
}

void IndicatorEntryWidget::updatePix()
{
    bool oldIsEmpty = isEmpty();

    int width = m_padding;
    int iconX = m_padding;
    int labelX = 0;

    // Compute width, labelX and update m_has{Icon,Label}
    QPixmap iconPix;
    if (m_entry->image_visible()) {
        iconPix = decodeIcon();
        m_hasIcon = !iconPix.isNull();
    } else {
        m_hasIcon = false;
    }
    if (m_hasIcon) {
        width += iconPix.width();
    }

    QString label = QString::fromUtf8(m_entry->label().c_str());
    label = swapMnemonicChar(label, '_', '&');
    m_hasLabel = !label.isEmpty() && m_entry->label_visible();
    if (m_hasLabel) {
        if (m_hasIcon) {
            width += SPACING;
        }
        labelX = width;
        QString visibleLabel = label;
        visibleLabel.remove('&');
        width += fontMetrics().width(visibleLabel);
    }

    width += m_padding;

    // Paint
    QPixmap oldPix = m_pix;
    if (!m_hasIcon && !m_hasLabel) {
        m_pix = QPixmap();
    } else {
        m_pix = QPixmap(width, 24);
        m_pix.fill(Qt::transparent);
        QPainter painter(&m_pix);
        painter.initFrom(this);
        if (m_hasIcon) {
            bool disabled = !m_entry->image_sensitive();
            if (disabled) {
                painter.setOpacity(0.5);
            }
            painter.drawPixmap(iconX, 0, iconPix);
            if (disabled) {
                painter.setOpacity(1);
            }
        }
        if (m_hasLabel) {
            PanelStyle* style = PanelStyle::instance();

            int flags = Qt::AlignLeft | Qt::AlignVCenter;
            flags |= m_entry->show_now() ? Qt::TextShowMnemonic : Qt::TextHideMnemonic;

            // Shadow
            QColor color = style->textShadowColor();
            color.setAlphaF(1. - color.redF());
            painter.setPen(color);
            painter.drawText(labelX, 1, width - labelX, m_pix.height(), flags, label);

            // Text
            color = style->textColor();
            color.setAlphaF(m_entry->label_sensitive() ? 1. : .5);
            painter.setPen(color);
            painter.drawText(labelX, 0, width - labelX, m_pix.height(), flags, label);
        }
    }

    // Notify others we changed, but only trigger a layout update if necessary
    if (m_pix.size() == oldPix.size()) {
        update();
    } else {
        updateGeometry();
    }
    bool newIsEmpty = isEmpty();
    if (newIsEmpty != oldIsEmpty) {
        // If we emit isEmptyChanged() directly it won't reach any connected
        // slot. I assume this is because this method is called as a response
        // to a sigc++ signal.
        QMetaObject::invokeMethod(this, "isEmptyChanged", Qt::QueuedConnection);
    }
}

QPixmap IndicatorEntryWidget::decodeIcon()
{
    QPixmap pix;

    int type = m_entry->image_type();

    if (type == 0) {
        // No icon
    } else if (type == GTK_IMAGE_PIXBUF) {
        QByteArray data = QByteArray::fromBase64(m_entry->image_data().c_str());
        QImage image;
        bool ok = image.loadFromData(data);
        if (ok) {
            pix = QPixmap::fromImage(image);
        } else {
            UQ_WARNING << "Failed to decode image";
        }
    } else if (type == GTK_IMAGE_ICON_NAME) {
        QString name = QString::fromStdString(m_entry->image_data());
        QIcon icon = QIcon::fromTheme(name);
        pix = icon.pixmap(24, 24);
    } else if (type == GTK_IMAGE_GICON) {
        UQ_WARNING << "FIXME: Implement support for GTK_IMAGE_GICON image type";
    } else {
        UQ_WARNING << "Unknown image type" << m_entry->image_type();
    }
    return pix;
}

void IndicatorEntryWidget::mousePressEvent(QMouseEvent*)
{
    UQ_VAR(this);
    UQ_RETURN_IF_FAIL(m_hasIcon || m_hasLabel);
    showMenu(Qt::LeftButton);
}

void IndicatorEntryWidget::mouseReleaseEvent(QMouseEvent*)
{
    UQ_VAR(this);
    update();
}

void IndicatorEntryWidget::wheelEvent(QWheelEvent* event)
{
    m_entry->Scroll(event->delta());
}

void IndicatorEntryWidget::showMenu(Qt::MouseButton qtButton)
{
    if (m_entry->active()) {
        return;
    }
    int nuxButton = qtButton == Qt::NoButton ? 0 : 1;
    QPoint pos = mapToGlobal(rect().bottomLeft());
    m_entry->ShowMenu(pos.x(), pos.y(),
        time(NULL),
        nuxButton
        );
}

void IndicatorEntryWidget::setPadding(int padding)
{
    if (m_padding != padding) {
        m_padding = padding;
        updatePix();
    }
}

bool IndicatorEntryWidget::event(QEvent* ev)
{
    bool ret = QWidget::event(ev);
    switch (ev->type()) {
    case QEvent::FontChange:
    case QEvent::PaletteChange:
        updatePix();
        break;
    default:
        break;
    }
    return ret;
}

bool IndicatorEntryWidget::isEmpty() const
{
    return !m_hasIcon && !m_hasLabel;
}

unity::indicator::Entry::Ptr IndicatorEntryWidget::entry() const
{
    return m_entry;
}

#include "indicatorentrywidget.moc"
