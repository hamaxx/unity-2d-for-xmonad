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
#include "croppedlabel.h"

// Local
#include <cairoutils.h>
#include <panelstyle.h>

// unity-2d
#include <debug_p.h>

// libqtgconf
#include <gconfitem-qml-wrapper.h>

// Qt
#include <QImage>
#include <QPainter>

// GTK
#include <gtk/gtk.h>

static const int FADEOUT_WIDTH = 30;

#define WINDOW_TITLE_FONT_KEY "/apps/metacity/general/titlebar_font"

CroppedLabel::CroppedLabel(QWidget* parent)
: QLabel(parent)
{
}

QSize CroppedLabel::minimumSizeHint() const
{
    return QWidget::minimumSizeHint();
}

static void paintFadeoutGradient(QImage* image)
{
    QPainter painter(image);
    painter.setCompositionMode(QPainter::CompositionMode_DestinationIn);
    QRect gradientRect(image->width() - FADEOUT_WIDTH, 0, FADEOUT_WIDTH, image->height());
    QLinearGradient gradient(gradientRect.topLeft(), gradientRect.topRight());
    gradient.setColorAt(0, Qt::white);
    gradient.setColorAt(1, Qt::transparent);
    painter.fillRect(gradientRect, gradient);
}

void CroppedLabel::paintEvent(QPaintEvent* event)
{
    QImage image(width(), height(), QImage::Format_ARGB32_Premultiplied);
    {
        QPainter painter(&image);
        painter.initFrom(this);
        painter.eraseRect(rect());
    }

    PangoLayout          *layout = NULL;
    PangoFontDescription *desc = NULL;
    GtkSettings          *settings = gtk_settings_get_default ();
    QByteArray            font_description;
    GdkScreen            *screen = gdk_screen_get_default ();
    int                   dpi = 0;

    int  text_width = 0;
    int  text_height = 0;

    GConfItemQmlWrapper client;
    client.setKey(WINDOW_TITLE_FONT_KEY);
    PangoContext *cxt;
    PangoRectangle log_rect;

    CairoUtils::SurfacePointer surface(CairoUtils::createSurfaceForQImage(&image));
    CairoUtils::Pointer cr(cairo_create(surface.data()));

    g_object_get (settings,
                  "gtk-xft-dpi", &dpi,
                  NULL);

    font_description = client.getValue().toString().toUtf8();
    desc = pango_font_description_from_string (font_description.data());

    layout = pango_cairo_create_layout (cr.data());
    pango_layout_set_font_description (layout, desc);
    QByteArray utf8Text = text().toUtf8();
    pango_layout_set_text (layout, utf8Text.data(), -1);

    cxt = pango_layout_get_context (layout);
    pango_cairo_context_set_font_options (cxt, gdk_screen_get_font_options (screen));
    pango_cairo_context_set_resolution (cxt, (float)dpi/(float)PANGO_SCALE);
    pango_layout_context_changed (layout);

    pango_layout_get_extents (layout, NULL, &log_rect);
    text_width = log_rect.width / PANGO_SCALE;
    text_height = log_rect.height / PANGO_SCALE;

    pango_font_description_free (desc);


    cairo_set_operator (cr.data(), CAIRO_OPERATOR_OVER);

    cairo_set_line_width (cr.data(), 1);

    PanelStyle *style = PanelStyle::instance();
    GtkStyleContext *style_context = style->styleContext();

    gtk_style_context_save (style_context);

    GtkWidgetPath *widget_path = gtk_widget_path_new ();
    gtk_widget_path_append_type (widget_path, GTK_TYPE_MENU_BAR);
    gtk_widget_path_append_type (widget_path, GTK_TYPE_MENU_ITEM);
    gtk_widget_path_iter_set_name (widget_path, -1 , "UnityPanelWidget");

    gtk_style_context_set_path (style_context, widget_path);
    gtk_style_context_add_class (style_context, GTK_STYLE_CLASS_MENUBAR);
    gtk_style_context_add_class (style_context, GTK_STYLE_CLASS_MENUITEM);

    pango_cairo_update_layout (cr.data(), layout);

    gtk_render_layout (style_context, cr.data(),
        contentsRect().left(),
        contentsRect().top() + (height() - text_height) / 2,
        layout);

    gtk_widget_path_free (widget_path);

    gtk_style_context_restore (style_context);

    if (text_width > contentsRect().width()) {
        paintFadeoutGradient(&image);
    }

    QPainter painter(this);
    painter.drawImage(0, 0, image);
}

#include "croppedlabel.moc"
