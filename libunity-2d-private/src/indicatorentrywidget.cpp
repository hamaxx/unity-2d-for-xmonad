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
#include <cairoutils.h>
#include <debug_p.h>
#include <gscopedpointer.h>
#include <gimageutils.h>
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
static const int PADDING = 5;
static const int ICON_SIZE = 22;

using namespace unity::indicator;

IndicatorEntryWidget::IndicatorEntryWidget(const Entry::Ptr& entry)
: m_entry(entry)
, m_padding(PADDING)
, m_hasIcon(false)
, m_hasLabel(false)
, m_gtkWidgetPath(gtk_widget_path_new())
{
    gtk_widget_path_append_type(m_gtkWidgetPath, GTK_TYPE_WINDOW);
    gtk_widget_path_iter_set_name(m_gtkWidgetPath, -1 , "UnityPanelWidget");
    gtk_widget_path_append_type(m_gtkWidgetPath, GTK_TYPE_MENU_BAR);
    gtk_widget_path_append_type(m_gtkWidgetPath, GTK_TYPE_MENU_ITEM);

    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Minimum);
    m_entry->updated.connect(sigc::mem_fun(this, &IndicatorEntryWidget::updatePix));
}

IndicatorEntryWidget::~IndicatorEntryWidget()
{
    gtk_widget_path_free(m_gtkWidgetPath);
}

QSize IndicatorEntryWidget::minimumSizeHint() const
{
    return sizeHint();
}

QSize IndicatorEntryWidget::sizeHint() const
{
    return m_pix.size();
}

void IndicatorEntryWidget::resizeEvent(QResizeEvent* event)
{
    QWidget::resizeEvent(event);
    updatePix();
}

void IndicatorEntryWidget::paintEvent(QPaintEvent*)
{
    if (!m_pix.isNull()) {
        QPainter painter(this);
        painter.drawPixmap(0, 0, m_pix);
    }
}


void IndicatorEntryWidget::paintActiveBackground(QImage* image)
{
    // This code should be kept in sync with corresponding unityshell code from
    // plugins/unityshell/src/PanelIndicatorObjectEntryView.cpp

    // Get a surface and a context
    CairoUtils::SurfacePointer surface(CairoUtils::createSurfaceForQImage(image));
    CairoUtils::Pointer cr(cairo_create(surface.data()));

    // Init style
    GtkStyleContext* styleContext = PanelStyle::instance()->styleContext();

    gtk_style_context_save(styleContext);

    gtk_style_context_set_path(styleContext, m_gtkWidgetPath);
    gtk_style_context_add_class(styleContext, GTK_STYLE_CLASS_MENUBAR);
    gtk_style_context_add_class(styleContext, GTK_STYLE_CLASS_MENUITEM);
    gtk_style_context_set_state(styleContext, GTK_STATE_FLAG_PRELIGHT);

    // Draw
    // FIXME(Cimi) probably some padding is needed here.
    gtk_render_background(styleContext, cr.data(), 0, 0, width(), height());
    gtk_render_frame(styleContext, cr.data(), 0, 0, width(), height());

    // Clean up
    gtk_style_context_restore(styleContext);
}

void IndicatorEntryWidget::updatePix()
{
    bool oldIsEmpty = isEmpty();

    int width = m_padding;
    int iconX = m_padding;
    int labelX = 0;

    GObjectScopedPointer<PangoLayout> pangoLayout;

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

    m_hasLabel = !m_entry->label().empty() && m_entry->label_visible();
    if (m_hasLabel) {
        if (m_hasIcon) {
            width += SPACING;
        }
        labelX = width;
        pangoLayout.reset(createPangoLayout());
        int labelWidth;
        int labelHeight;
        pango_layout_get_pixel_size(pangoLayout.data(), &labelWidth, &labelHeight);

        width += labelWidth;
    }

    width += m_padding;

    // Paint
    QPixmap oldPix = m_pix;
    if (!m_hasIcon && !m_hasLabel) {
        m_pix = QPixmap();
    } else {
        QImage img(width, height(), QImage::Format_ARGB32_Premultiplied);
        QPainter painter(&img);
        painter.initFrom(this);
        painter.eraseRect(img.rect());
        if (m_entry->active()) {
            paintActiveBackground(&img);
        }
        if (m_hasIcon) {
            bool disabled = !m_entry->image_sensitive();
            if (disabled) {
                painter.setOpacity(0.5);
            }
            painter.drawPixmap(iconX, (height() - iconPix.height()) / 2, iconPix);
            if (disabled) {
                painter.setOpacity(1);
            }
        }
        if (m_hasLabel) {
            paintLabel(&img, pangoLayout.data(), labelX);
        }
        m_pix = QPixmap::fromImage(img);
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

PangoLayout* IndicatorEntryWidget::createPangoLayout()
{
    // Parse
    PangoAttrList* attrs = NULL;
    if (m_entry->show_now()) {
        if (!pango_parse_markup(m_entry->label().c_str(),
                                 -1,
                                 '_',
                                 &attrs,
                                 NULL,
                                 NULL,
                                 NULL))
        {
            UQ_WARNING << "pango_parse_markup failed";
        }
    }

    // Create layout
    GObjectScopedPointer<PangoContext> pangoContext(gdk_pango_context_get());
    PangoLayout* layout = pango_layout_new(pangoContext.data());

    if (attrs) {
        pango_layout_set_attributes(layout, attrs);
        pango_attr_list_unref(attrs);
    }

    // Set font
    char* font_description = NULL;
    GtkSettings *settings = gtk_settings_get_default();
    g_object_get(settings,
                  "gtk-font-name", &font_description,
                  NULL);
    PangoFontDescription* desc = pango_font_description_from_string(font_description);
    pango_font_description_set_weight(desc, PANGO_WEIGHT_NORMAL);
    pango_layout_set_font_description(layout, desc);
    pango_font_description_free(desc);
    g_free(font_description);

    // Set text
    QString label = QString::fromUtf8(m_entry->label().c_str());
    label.replace('_', QString());
    QByteArray utf8Label = label.toUtf8();
    pango_layout_set_text(layout, utf8Label.data(), -1);

    return layout;
}

void IndicatorEntryWidget::paintLabel(QImage* image, PangoLayout* layout, int labelX)
{
    // This code should be kept in sync with corresponding unityshell code from
    // plugins/unityshell/src/PanelIndicatorObjectEntryView.cpp
    int labelWidth, labelHeight;
    pango_layout_get_pixel_size(layout, &labelWidth, &labelHeight);
    CairoUtils::SurfacePointer surface(CairoUtils::createSurfaceForQImage(image));
    CairoUtils::Pointer cr(cairo_create(surface.data()));
    pango_cairo_update_layout(cr.data(), layout);

    PanelStyle* style = PanelStyle::instance();
    GtkStyleContext* styleContext = style->styleContext();

    gtk_style_context_save(styleContext);

    gtk_style_context_set_path(styleContext, m_gtkWidgetPath);
    gtk_style_context_add_class(styleContext, GTK_STYLE_CLASS_MENUBAR);
    gtk_style_context_add_class(styleContext, GTK_STYLE_CLASS_MENUITEM);

    if (m_entry->active()) {
        gtk_style_context_set_state(styleContext, GTK_STATE_FLAG_PRELIGHT);
    }

    gtk_render_layout(styleContext, cr.data(), labelX, (image->height() - labelHeight) / 2, layout);
    gtk_style_context_restore(styleContext);
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
        pix = icon.pixmap(ICON_SIZE, ICON_SIZE);
    } else if (type == GTK_IMAGE_GICON) {
        QString name = QString::fromStdString(m_entry->image_data());
        QImage image = GImageUtils::imageForIconString(name, ICON_SIZE);
        if (image.isNull()) {
            UQ_WARNING << "Failed to load icon from" << name;
            return QPixmap();
        }
        return QPixmap::fromImage(image);
    } else {
        UQ_WARNING << "Unknown image type" << m_entry->image_type();
    }
    return pix;
}

void IndicatorEntryWidget::mousePressEvent(QMouseEvent* event)
{
    UQ_RETURN_IF_FAIL(m_hasIcon || m_hasLabel);
    if (event->button() != Qt::MiddleButton)
	    showMenu(Qt::LeftButton);
}

void IndicatorEntryWidget::mouseReleaseEvent(QMouseEvent* event)
{
    UQ_VAR(this);
    update();

    if (event->button() == Qt::MiddleButton && rect().contains(event->pos(), false))
    	m_entry->SecondaryActivate(time(NULL));
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
